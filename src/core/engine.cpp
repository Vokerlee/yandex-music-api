/*
 * Copyright (c) 2026 Vokerlee <vokerlee@gmail.com>
 *
 * Yandex Music Native is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as published
 * by the Free Software Foundation.
 *
 * Yandex Music Native is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "engine.hpp"
#include "client.hpp"
#include "error.hpp"
#include <algorithm>
#include <cstring>
#include <limits>
#include <new>

namespace ym::detail
{
namespace
{
bool pending(const request_state& state) noexcept
{
    return state.phase == YM_REQUEST_QUEUED || state.phase == YM_REQUEST_RUNNING;
}
void finish(request_state& state, ym_status status, std::string_view message) noexcept
{
    state.phase = status == YM_OK ? YM_REQUEST_SUCCEEDED
        : status == YM_CANCELLED  ? YM_REQUEST_CANCELLED
                                  : YM_REQUEST_FAILED;
    fail(&state.failure, status, message);
}
void cancel(request_state& state, ym_status reason) noexcept
{
    if (!pending(state))
        return;
    const bool started = state.phase == YM_REQUEST_RUNNING;
    if (started)
        if (auto owner = state.owner.lock())
            if (owner->adapter)
                owner->adapter->cancel(state.ticket);
    if (started && state.input.mutating)
        reason = YM_OUTCOME_UNKNOWN;
    finish(state, reason,
        reason == YM_OUTCOME_UNKNOWN ? "Server-side outcome is unknown"
            : reason == YM_CANCELLED ? "Request cancelled"
                                     : "Request deadline exceeded");
}
void close(engine& owner) noexcept
{
    owner.closed = true;
    for (auto& weak : owner.requests)
        if (auto state = weak.lock()) {
            cancel(*state, YM_CANCELLED);
            state->suppressed = true;
            state->options.on_complete = nullptr;
        }
    owner.requests.clear();
}
void cleanup(engine& owner)
{
    std::erase_if(owner.requests, [](const auto& weak) {
        const auto state = weak.lock();
        return !state || (!pending(*state) && (state->delivered || state->suppressed));
    });
}
ym_status drive(engine& owner, uint64_t now, size_t budget, ym_error* error)
{
    size_t used = 0;
    const size_t count = owner.requests.size();
    for (size_t i = 0; i < count && used < budget; ++i) {
        auto state = owner.requests[i].lock();
        if (!state || !pending(*state))
            continue;
        if (state->options.deadline_ms != UINT64_MAX && now >= state->options.deadline_ms) {
            cancel(*state, YM_DEADLINE_EXCEEDED);
            ++used;
            continue;
        }
        if (state->phase == YM_REQUEST_QUEUED) {
            try {
                owner.adapter->start(state->ticket, state->input);
                state->phase = YM_REQUEST_RUNNING;
            } catch (const std::bad_alloc&) {
                finish(*state, YM_OUT_OF_MEMORY, "Transport allocation failed");
            } catch (...) {
                finish(*state, YM_NETWORK_ERROR, "Transport could not start");
            }
            ++used;
        }
    }
    while (used < budget && owner.adapter) {
        auto completion = owner.adapter->poll(now);
        if (!completion)
            break;
        ++used;
        for (auto& weak : owner.requests) {
            auto state = weak.lock();
            if (!state || state->ticket != completion->ticket || state->phase != YM_REQUEST_RUNNING)
                continue;
            if (completion->status != YM_OK)
                finish(*state,
                    state->input.mutating ? static_cast<ym_status>(YM_OUTCOME_UNKNOWN)
                                          : completion->status,
                    "Transport failed; server-side outcome may be unknown");
            else if (completion->body.size() > state->max_response_bytes)
                finish(*state, YM_RESOURCE_LIMIT, "Response body exceeds configured limit");
            else {
                ym_result* result = nullptr;
                const auto status = state->decode(*completion, &result, &state->failure);
                state->result.reset(result);
                state->phase = status == YM_OK ? YM_REQUEST_SUCCEEDED : YM_REQUEST_FAILED;
                if (status == YM_OK && !result)
                    finish(*state, YM_INTERNAL_ERROR, "Decoder returned no result");
                else if (status != YM_OK && state->failure.code != status)
                    fail(&state->failure, status, "Response decoding failed");
            }
            break;
        }
    }
    ym_status delivery_status = YM_OK;
    for (size_t i = 0; i < count && used < budget; ++i) {
        auto state = owner.requests[i].lock();
        if (!state || pending(*state) || state->delivered || state->suppressed)
            continue;
        state->delivered = true;
        if (auto callback = state->options.on_complete) {
            auto* handle = state->handle;
            handle->in_callback = true;
            try {
                callback(handle, state->failure.code, state->options.userdata);
            } catch (...) {
                delivery_status
                    = fail(error, YM_INTERNAL_ERROR, "Completion callback threw an exception");
            }
            handle->in_callback = false;
            if (handle->destroy_requested)
                delete handle;
        }
        ++used;
        if (owner.stopping)
            break;
    }
    cleanup(owner);
    return delivery_status;
}
} // namespace

ym_status submit(ym_client* client, request_input input, decoder_fn decode,
    const ym_request_options* input_options, ym_request** out, ym_error* error) noexcept
{
    clear(error);
    if (!out)
        return fail(error, YM_INVALID_ARGUMENT, "Request output is required");
    *out = nullptr;
    if (!client || !decode)
        return fail(error, YM_INVALID_ARGUMENT, "Client and decoder are required");
    auto owner = client->execution;
    if (owner->closed || client->destroy_requested)
        return fail(error, YM_CLOSED, "Client is closed");
    if (!owner->adapter)
        return fail(error, YM_UNSUPPORTED, "No transport adapter is configured");
    try {
        ym_request_options options;
        ym_request_options_init(&options);
        if (input_options) {
            size_t size = 0;
            std::memcpy(&size, input_options, sizeof(size));
            if (size < offsetof(ym_request_options, deadline_ms))
                return fail(error, YM_INVALID_ARGUMENT, "Request options prefix is too small");
            uint32_t version = 0;
            std::memcpy(&version,
                reinterpret_cast<const char*>(input_options)
                    + offsetof(ym_request_options, version),
                sizeof(version));
            if (version != 1)
                return fail(error, YM_UNSUPPORTED, "Unsupported request options version");
#define YM_READ(field)                                                                             \
    do {                                                                                           \
        if (size >= offsetof(ym_request_options, field) + sizeof(options.field))                   \
            std::memcpy(&options.field,                                                            \
                reinterpret_cast<const char*>(input_options)                                       \
                    + offsetof(ym_request_options, field),                                         \
                sizeof(options.field));                                                            \
    } while (false)
            YM_READ(deadline_ms);
            YM_READ(on_complete);
            YM_READ(userdata);
#undef YM_READ
        }
        if (!owner->driving)
            cleanup(*owner);
        const auto active
            = std::count_if(owner->requests.begin(), owner->requests.end(), [](const auto& weak) {
                  const auto state = weak.lock();
                  return state && !state->suppressed && (pending(*state) || !state->delivered);
              });
        if (static_cast<size_t>(active) >= client->max_pending_requests || owner->next_ticket == 0)
            return fail(error, YM_RESOURCE_LIMIT, "Pending request limit exceeded");
        auto state = std::make_shared<request_state>();
        state->owner = owner;
        state->input = std::move(input);
        state->decode = decode;
        state->options = options;
        state->ticket = owner->next_ticket++;
        state->max_response_bytes = client->max_response_bytes;
        ym_error_init(&state->failure);
        auto handle = std::make_unique<ym_request>();
        handle->state = state;
        state->handle = handle.get();
        owner->requests.push_back(state);
        *out = handle.release();
        return YM_OK;
    } catch (const std::bad_alloc&) {
        return fail(error, YM_OUT_OF_MEMORY, "Request allocation failed");
    } catch (...) {
        return fail(error, YM_INTERNAL_ERROR, "Request submission failed");
    }
}
} // namespace ym::detail

void YM_CALL ym_request_options_init(ym_request_options* options)
{
    if (!options)
        return;
    *options = { };
    options->struct_size = sizeof(*options);
    options->version = 1;
    options->deadline_ms = UINT64_MAX;
}
ym_status YM_CALL ym_client_close(ym_client* client, ym_error* error)
{
    using namespace ym::detail;
    clear(error);
    if (!client)
        return fail(error, YM_INVALID_ARGUMENT, "Client is required");
    if (client->execution->driving)
        return fail(error, YM_REENTRANT_CALL, "Client is delivering work");
    close(*client->execution);
    return YM_OK;
}
void YM_CALL ym_client_destroy(ym_client* client)
{
    if (!client)
        return;
    if (client->execution->driving) {
        client->destroy_requested = true;
        client->execution->stopping = true;
        return;
    }
    ym_client_close(client, nullptr);
    delete client;
}
ym_status YM_CALL ym_client_poll(ym_client* client, uint64_t now, size_t budget, ym_error* error)
{
    using namespace ym::detail;
    clear(error);
    if (!client || budget == 0)
        return fail(error, YM_INVALID_ARGUMENT, "Client and positive work budget are required");
    auto owner = client->execution;
    if (owner->driving)
        return fail(error, YM_REENTRANT_CALL, "Recursive polling is not allowed");
    if (owner->closed)
        return fail(error, YM_CLOSED, "Client is closed");
    if (owner->has_time && now < owner->last_time)
        return fail(error, YM_INVALID_ARGUMENT, "Monotonic time moved backwards");
    owner->last_time = now;
    owner->has_time = true;
    owner->driving = true;
    ym_status status = YM_OK;
    try {
        status = drive(*owner, now, budget, error);
    } catch (const std::bad_alloc&) {
        status = fail(error, YM_OUT_OF_MEMORY, "Poll allocation failed");
    } catch (...) {
        status = fail(error, YM_INTERNAL_ERROR, "Polling failed");
    }
    owner->driving = false;
    if (client->destroy_requested)
        ym_client_destroy(client);
    return status;
}
ym_request_state YM_CALL ym_request_get_state(const ym_request* request)
{
    return request ? request->state->phase : static_cast<ym_request_state>(YM_REQUEST_INVALID);
}
ym_status YM_CALL ym_request_get_status(const ym_request* request)
{
    if (!request)
        return YM_INVALID_ARGUMENT;
    return ym::detail::pending(*request->state) ? static_cast<ym_status>(YM_PENDING)
                                                : request->state->failure.code;
}
ym_status YM_CALL ym_request_cancel(ym_request* request, ym_error* error)
{
    ym::detail::clear(error);
    if (!request)
        return ym::detail::fail(error, YM_INVALID_ARGUMENT, "Request is required");
    ym::detail::cancel(*request->state, YM_CANCELLED);
    return YM_OK;
}
ym_status YM_CALL ym_request_take_result(ym_request* request, ym_result** out, ym_error* error)
{
    using namespace ym::detail;
    clear(error);
    if (!out)
        return fail(error, YM_INVALID_ARGUMENT, "Result output is required");
    *out = nullptr;
    if (!request)
        return fail(error, YM_INVALID_ARGUMENT, "Request is required");
    auto& state = *request->state;
    if (pending(state))
        return fail(error, YM_PENDING, "Request is pending");
    if (state.failure.code != YM_OK) {
        if (error)
            *error = state.failure;
        return state.failure.code;
    }
    if (!state.result)
        return fail(error, YM_INVALID_STATE, "Result was already taken");
    *out = state.result.release();
    return YM_OK;
}
void YM_CALL ym_request_destroy(ym_request* request)
{
    if (!request)
        return;
    ym::detail::cancel(*request->state, YM_CANCELLED);
    request->state->suppressed = true;
    request->state->options.on_complete = nullptr;
    request->state->handle = nullptr;
    if (request->in_callback) {
        request->destroy_requested = true;
        return;
    }
    delete request;
}
