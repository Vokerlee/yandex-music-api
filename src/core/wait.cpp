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
#include "error.hpp"

#include <algorithm>
#include <chrono>
#include <thread>

uint64_t YM_CALL ym_monotonic_ms(void) YM_NOEXCEPT
{
    const auto now = std::chrono::steady_clock::now().time_since_epoch();
    return static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(now).count()
    );
}

ym_status YM_CALL ym_client_wait(ym_client* client, ym_request* request, uint64_t timeout_ms,
    ym_result** out, ym_error* error) YM_NOEXCEPT
{
    using namespace ym::detail;
    using Clock = std::chrono::steady_clock;
    clear(error);
    if (out)
        *out = nullptr;
    if (!client || !request || !out)
        return fail(error, YM_INVALID_ARGUMENT, "Client, request and result output are required");
    auto state = request->state;
    auto owner = state->owner.lock();
    if (owner != client->execution)
        return fail(error, YM_INVALID_ARGUMENT, "Request belongs to another client");
    if (owner->driving)
        return fail(error, YM_REENTRANT_CALL, "Cannot wait during callback delivery");

    const auto started = Clock::now();
    try {
        while (pending(*state)) {
            const auto now = Clock::now();
            const auto elapsed =
                std::chrono::duration_cast<std::chrono::milliseconds>(now - started).count();
            if (timeout_ms != UINT64_MAX && static_cast<uint64_t>(elapsed) >= timeout_ms) {
                cancel_state(*state, YM_DEADLINE_EXCEEDED);
                break;
            }
            const auto ticks = ym_monotonic_ms();
            const auto status =
                ym_client_poll(client, std::max(owner->last_time, ticks), 64, error);
            if (status != YM_OK)
                return status;
            if (pending(*state))
                std::this_thread::sleep_for(std::chrono::milliseconds{1});
        }
    } catch (...) {
        return fail(error, YM_INTERNAL_ERROR, "Waiting for request failed");
    }
    if (state->failure.code != YM_OK) {
        if (error)
            *error = state->failure;
        return state->failure.code;
    }
    if (!state->result)
        return fail(error, YM_INVALID_STATE, "Result was already taken");
    *out = state->result.release();
    return YM_OK;
}
