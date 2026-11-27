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

#ifndef YANDEX_MUSIC_REQUEST_H
#define YANDEX_MUSIC_REQUEST_H

#include <yandex_music/client.h>
#include <yandex_music/result.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ym_request ym_request;

typedef uint32_t ym_request_state;

enum {
    YM_REQUEST_INVALID = 0,
    YM_REQUEST_QUEUED = 1,
    YM_REQUEST_RUNNING = 2,
    YM_REQUEST_SUCCEEDED = 3,
    YM_REQUEST_FAILED = 4,
    YM_REQUEST_CANCELLED = 5,
};

/** Delivery is on the polling thread, once. Request destruction inside delivery is deferred.
 * Starting new work is allowed, but recursive poll/close returns YM_REENTRANT_CALL. */
typedef void(YM_CALL* ym_completion_fn)(ym_request* request, ym_status status, void* userdata);

typedef struct ym_request_options {
    size_t struct_size;
    uint32_t version;
    uint64_t deadline_ms; /* absolute monotonic time; UINT64_MAX means no deadline */
    ym_completion_fn on_complete;
    void* userdata;
} ym_request_options;

YM_API void YM_CALL ym_request_options_init(ym_request_options* options) YM_NOEXCEPT;

/** Advances ready work without sleeping. Time must not move backwards; budget must be >0. */
YM_API ym_status YM_CALL ym_client_poll(ym_client* client, uint64_t monotonic_ms,
    size_t work_budget, ym_error* error) YM_NOEXCEPT;

/** Cancels locally and suppresses pending callbacks. Repeated close is harmless. */
YM_API ym_status YM_CALL ym_client_close(ym_client* client, ym_error* error) YM_NOEXCEPT;

YM_API ym_request_state YM_CALL ym_request_get_state(const ym_request* request) YM_NOEXCEPT;

YM_API ym_status YM_CALL ym_request_get_status(const ym_request* request) YM_NOEXCEPT;

/** Idempotent. Started mutations may report YM_OUTCOME_UNKNOWN. Delivery waits for poll. */

/** Result transfer is exactly once; outputs reset on every failure, including YM_PENDING. */
YM_API ym_status YM_CALL ym_request_take_result(ym_request* request, ym_result** out,
    ym_error* error) YM_NOEXCEPT;

/** Cancels/detaches pending work and suppresses its callback. NULL is harmless. */
YM_API void YM_CALL ym_request_destroy(ym_request* request) YM_NOEXCEPT;

#ifdef __cplusplus
}
#endif

#endif
