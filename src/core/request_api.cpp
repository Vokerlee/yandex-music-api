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

#include "client.hpp"
#include "engine.hpp"
#include "error.hpp"

#include <cstring>
#include <new>

namespace ym::detail {

ym_status submit(ym_client* client, RequestInput input, ResponseDecoder decode,
    const ym_request_options* input_options, ym_request** out, ym_error* error) noexcept
{
    clear(error);
    if (!out)
        return fail(error, YM_INVALID_ARGUMENT, "Request output is required");
    *out = nullptr;
    if (!client)
        return fail(error, YM_INVALID_ARGUMENT, "Client is required");
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
                reinterpret_cast<const char*>(input_options) +
                    offsetof(ym_request_options, version),
                sizeof(version));
            if (version != 1)
                return fail(error, YM_UNSUPPORTED, "Unsupported request options version");
#define READ(field)                                                                                \
    if (size >= offsetof(ym_request_options, field) + sizeof(options.field))                       \
    std::memcpy(&options.field,                                                                    \
        reinterpret_cast<const char*>(input_options) + offsetof(ym_request_options, field),        \
        sizeof(options.field))
            READ(deadline_ms);
            READ(on_complete);
            READ(userdata);
#undef READ
        }
        auto handle = std::make_unique<ym_request>();
        auto queued = queue_state(client->execution, std::move(input), decode, options);
        if (!queued) {
            if (error)
                *error = queued.error();
            return queued.error().code;
        }
        handle->state = *queued;
        handle->state->handle = handle.get();
        *out = handle.release();
        return YM_OK;
    } catch (const std::bad_alloc&) {
        return fail(error, YM_OUT_OF_MEMORY, "Request handle allocation failed");
    } catch (...) {
        return fail(error, YM_INTERNAL_ERROR, "Request submission failed");
    }
}

} // namespace ym::detail

ym_status YM_CALL ym_client_close(ym_client* client, ym_error* error) YM_NOEXCEPT
{
    ym::detail::clear(error);
    if (!client)
        return ym::detail::fail(error, YM_INVALID_ARGUMENT, "Client is required");
    if (client->execution->driving)
        return ym::detail::fail(error, YM_REENTRANT_CALL, "Client is delivering work");
    ym::detail::close_engine(*client->execution);
    return YM_OK;
}

void YM_CALL ym_client_destroy(ym_client* client) YM_NOEXCEPT
{
    if (!client)
        return;
    /* DEFER_CLIENT_DESTRUCTION */
    ym_client_close(client, nullptr);
    delete client;
}

ym_status YM_CALL ym_client_poll(ym_client* client, uint64_t now, size_t budget,
    ym_error* error) YM_NOEXCEPT
{
    if (!client)
        return ym::detail::fail(error, YM_INVALID_ARGUMENT, "Client is required");
    auto owner = client->execution;
    const auto result = ym::detail::poll_engine(*owner, now, budget, error);
    /* FINISH_CLIENT_DESTRUCTION */
    return result;
}

ym_request_state YM_CALL ym_request_get_state(const ym_request* r) YM_NOEXCEPT
{
    return r ? r->state->phase : static_cast<ym_request_state>(YM_REQUEST_INVALID);
}

ym_status YM_CALL ym_request_get_status(const ym_request* r) YM_NOEXCEPT
{
    if (!r)
        return YM_INVALID_ARGUMENT;
    return ym::detail::pending(*r->state) ? static_cast<ym_status>(YM_PENDING)
                                          : r->state->failure.code;
}

ym_status YM_CALL ym_request_take_result(ym_request* r, ym_result** out,
    ym_error* error) YM_NOEXCEPT
{
    using namespace ym::detail;
    clear(error);
    if (!out)
        return fail(error, YM_INVALID_ARGUMENT, "Result output is required");
    *out = nullptr;
    if (!r)
        return fail(error, YM_INVALID_ARGUMENT, "Request is required");
    auto& s = *r->state;
    if (pending(s))
        return fail(error, YM_PENDING, "Request is pending");
    if (s.failure.code != YM_OK) {
        if (error)
            *error = s.failure;
        return s.failure.code;
    }
    if (!s.result)
        return fail(error, YM_INVALID_STATE, "Result was already taken");
    *out = s.result.release();
    return YM_OK;
}

void YM_CALL ym_request_destroy(ym_request* r) YM_NOEXCEPT
{
    if (!r)
        return;
    ym::detail::cancel_state(*r->state);
    r->state->suppressed = true;
    r->state->handle = nullptr;
    /* DEFER_REQUEST_DESTRUCTION */
    delete r;
}

ym_status YM_CALL ym_request_cancel(ym_request* request, ym_error* error) YM_NOEXCEPT
{
    ym::detail::clear(error);
    if (!request)
        return ym::detail::fail(error, YM_INVALID_ARGUMENT, "Request is required");
    ym::detail::cancel_state(*request->state);
    return YM_OK;
}
