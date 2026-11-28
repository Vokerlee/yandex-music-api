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

#ifndef YANDEX_MUSIC_TYPES_H
#define YANDEX_MUSIC_TYPES_H

#include <stddef.h>
#include <stdint.h>

#include <yandex_music/export.h>

/** UTF-8 byte span. NULL is valid only when size is zero. */
typedef struct ym_string_view {
    const char* data;
    size_t size;
} ym_string_view;

typedef uint32_t ym_status;

enum {
    YM_OK = 0,
    YM_INVALID_ARGUMENT = 1,
    YM_OUT_OF_MEMORY = 2,
    YM_CLOSED = 3,
    YM_PENDING = 4,
    YM_CANCELLED = 5,
    YM_DEADLINE_EXCEEDED = 6,
    YM_RESOURCE_LIMIT = 7,
    YM_SCHEMA_ERROR = 8,
    YM_SERVICE_ERROR = 9,
    YM_UNAUTHORIZED = 10,
    YM_FORBIDDEN = 11,
    YM_NOT_FOUND = 12,
    YM_RATE_LIMITED = 13,
    YM_CONFLICT = 14,
    YM_NETWORK_ERROR = 15,
    YM_TLS_ERROR = 16,
    YM_UNSUPPORTED = 17,
    YM_OUTCOME_UNKNOWN = 18,
    YM_INTERNAL_ERROR = 19,
    YM_INVALID_STATE = 20,
    YM_REENTRANT_CALL = 21,
    YM_IO_ERROR = 22,
};
#endif
