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
#include "error.hpp"

#include <algorithm>
#include <new>

namespace ym::detail {

bool pending(const RequestState& s) noexcept
{
    return s.phase == YM_REQUEST_QUEUED || s.phase == YM_REQUEST_RUNNING;
}

void finish(RequestState& s, ym_status status, const char* message) noexcept
{
    s.phase = status == YM_OK    ? YM_REQUEST_SUCCEEDED
        : status == YM_CANCELLED ? YM_REQUEST_CANCELLED
                                 : YM_REQUEST_FAILED;
    fail(&s.failure, status, message);
}

void cancel_state(RequestState& s, ym_status reason, Engine* context) noexcept
{
    if (!pending(s))
        return;
    const bool started = s.phase == YM_REQUEST_RUNNING;
    if (started) {
        auto owner = s.owner.lock();
        auto* active = context ? context : owner.get();
        if (active && active->adapter)
            active->adapter->cancel(s.ticket);
    }
    if (started && s.input.mutating)
        reason = YM_OUTCOME_UNKNOWN;
    finish(s, reason, "Request cancelled or expired");
}

void close_engine(Engine& owner) noexcept
{
    owner.closed = true;
    for (auto& s : owner.requests) {
        cancel_state(*s, YM_CANCELLED, &owner);
        s->suppressed = true;
    }
    owner.requests.clear();
}

Engine::~Engine()
{
    close_engine(*this);
}

std::expected<std::shared_ptr<RequestState>, ym_error>
queue_state(const std::shared_ptr<Engine>& owner, RequestInput input, ResponseDecoder decode,
    const ym_request_options& options) noexcept
{
    ym_error error;
    if (!owner || !decode) {
        fail(&error, YM_INVALID_ARGUMENT, "Engine and decoder are required");
        return std::unexpected(error);
    }
    if (owner->closed) {
        fail(&error, YM_CLOSED, "Client is closed");
        return std::unexpected(error);
    }
    if (!owner->adapter) {
        fail(&error, YM_UNSUPPORTED, "No transport adapter is configured");
        return std::unexpected(error);
    }
    try {
        if (!owner->driving)
            std::erase_if(owner->requests,
                [](const auto& s) { return !pending(*s) && (s->delivered || s->suppressed); });
        const auto active = std::count_if(owner->requests.begin(), owner->requests.end(),
            [](const auto& s) { return pending(*s) || (!s->delivered && !s->suppressed); });
        if (static_cast<size_t>(active) >= owner->max_pending || owner->next_ticket == 0) {
            fail(&error, YM_RESOURCE_LIMIT, "Pending request limit exceeded");
            return std::unexpected(error);
        }
        auto s = std::make_shared<RequestState>();
        s->owner = owner;
        s->input = std::move(input);
        s->decode = decode;
        s->options = options;
        s->ticket = owner->next_ticket++;
        ym_error_init(&s->failure);
        owner->requests.push_back(s);
        return s;
    } catch (const std::bad_alloc&) {
        fail(&error, YM_OUT_OF_MEMORY, "Request allocation failed");
        return std::unexpected(error);
    } catch (...) {
        fail(&error, YM_INTERNAL_ERROR, "Request submission failed");
        return std::unexpected(error);
    }
}

ym_status poll_engine(Engine& owner, uint64_t now, size_t budget, ym_error* error) noexcept
{
    clear(error);
    if (!budget)
        return fail(error, YM_INVALID_ARGUMENT, "Work budget must be positive");
    if (owner.driving)
        return fail(error, YM_REENTRANT_CALL, "Recursive polling is not allowed");
    if (owner.closed)
        return fail(error, YM_CLOSED, "Client is closed");
    if (owner.has_time && now < owner.last_time)
        return fail(error, YM_INVALID_ARGUMENT, "Monotonic time moved backwards");
    owner.last_time = now;
    owner.has_time = true;
    owner.driving = true;
    ym_status status = YM_OK;
    try {
        size_t used = 0;
        const size_t count = owner.requests.size();
        for (size_t i = 0; i < count && used < budget; ++i) {
            auto s = owner.requests[i];
            if (!pending(*s))
                continue;
            if (s->options.deadline_ms != UINT64_MAX && now >= s->options.deadline_ms) {
                cancel_state(*s, YM_DEADLINE_EXCEEDED);
                ++used;
                continue;
            }
            if (s->phase == YM_REQUEST_QUEUED) {
                try {
                    owner.adapter->start(s->ticket, s->input);
                    s->phase = YM_REQUEST_RUNNING;
                } catch (const std::bad_alloc&) {
                    finish(*s, YM_OUT_OF_MEMORY, "Transport allocation failed");
                } catch (...) {
                    finish(*s, YM_NETWORK_ERROR, "Transport could not start");
                }
                ++used;
            }
        }
        while (used < budget && owner.adapter) {
            auto response = owner.adapter->poll(now);
            if (!response)
                break;
            ++used;
            for (auto& s : owner.requests) {
                if (s->ticket != response->ticket || s->phase != YM_REQUEST_RUNNING)
                    continue;
                if (response->status != YM_OK)
                    finish(*s,
                        s->input.mutating ? static_cast<ym_status>(YM_OUTCOME_UNKNOWN)
                                          : response->status,
                        "Transport failed");
                else if (response->body.size() > owner.max_response_bytes)
                    finish(*s, YM_RESOURCE_LIMIT, "Response body exceeds configured limit");
                else {
                    ym_result* raw = nullptr;
                    const auto code = s->decode(*response, &raw, &s->failure);
                    s->result.reset(raw);
                    s->phase = code == YM_OK ? YM_REQUEST_SUCCEEDED : YM_REQUEST_FAILED;
                    if (code == YM_OK && !raw)
                        finish(*s, YM_INTERNAL_ERROR, "Decoder returned no result");
                    else if (code != YM_OK && s->failure.code != code)
                        fail(&s->failure, code, "Response decoding failed");
                }
                break;
            }
        }
        for (size_t i = 0; i < count && used < budget; ++i) {
            auto s = owner.requests[i];
            if (pending(*s) || s->delivered || s->suppressed)
                continue;
            s->delivered = true;
            if (auto callback = s->options.on_complete) {
                auto* handle = s->handle;
                if (!handle) {
                    status = fail(error, YM_INTERNAL_ERROR, "Callback handle is missing");
                    continue;
                }
                handle->in_callback = true;
                try {
                    callback(handle, s->failure.code, s->options.userdata);
                } catch (...) {
                    status = fail(error, YM_INTERNAL_ERROR, "Completion callback threw");
                }
                handle->in_callback = false;
                if (handle->destroy_requested)
                    delete handle;
            }
            ++used;
            if (owner.stopping)
                break;
        }
        std::erase_if(owner.requests,
            [](const auto& s) { return !pending(*s) && (s->delivered || s->suppressed); });
    } catch (const std::bad_alloc&) {
        status = fail(error, YM_OUT_OF_MEMORY, "Poll allocation failed");
    } catch (...) {
        status = fail(error, YM_INTERNAL_ERROR, "Polling failed");
    }
    owner.driving = false;
    return status;
}

} // namespace ym::detail
