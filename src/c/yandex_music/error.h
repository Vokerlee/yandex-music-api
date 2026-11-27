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

#ifndef YANDEX_MUSIC_ERROR_H
#define YANDEX_MUSIC_ERROR_H

#include <yandex_music/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Owned value; messages are bounded, NUL-terminated and never use global storage.
 * Initialize through ym_error_init. A NULL error output suppresses detail only. */
typedef struct ym_error {
    ym_status code;
    uint32_t http_status;
    int64_t retry_after_ms; /* -1 when unknown */
    char message[256];
    char server_code[64];
    char request_id[96];
} ym_error;

YM_API void YM_CALL ym_error_init(ym_error* error);

YM_API const char* YM_CALL ym_status_name(ym_status status);

#ifdef __cplusplus
}
#endif

#endif
