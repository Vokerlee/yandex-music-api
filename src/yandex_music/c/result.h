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

#ifndef YANDEX_MUSIC_RESULT_H
#define YANDEX_MUSIC_RESULT_H

#include <yandex_music/c/error.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ym_result ym_result;

typedef struct ym_value ym_value;

typedef uint32_t ym_value_type;

enum {
    YM_VALUE_MISSING = 0,
    YM_VALUE_NULL = 1,
    YM_VALUE_BOOL = 2,
    YM_VALUE_INT64 = 3,
    YM_VALUE_UINT64 = 4,
    YM_VALUE_DOUBLE = 5,
    YM_VALUE_STRING = 6,
    YM_VALUE_ARRAY = 7,
    YM_VALUE_OBJECT = 8,
};

/** Immutable result owns all nested views independently of client/request lifetime. */
YM_API void YM_CALL ym_result_destroy(ym_result* result) YM_NOEXCEPT;

YM_API ym_status YM_CALL ym_result_clone(const ym_result* input, ym_result** out,
    ym_error* error) YM_NOEXCEPT;

YM_API const ym_value* YM_CALL ym_result_root(const ym_result* result) YM_NOEXCEPT;

YM_API uint32_t YM_CALL ym_result_http_status(const ym_result* result) YM_NOEXCEPT;

YM_API ym_string_view YM_CALL ym_result_request_id(const ym_result* result) YM_NOEXCEPT;

/** NULL view is missing, distinct from JSON null and empty/zero/false values. */
YM_API ym_value_type YM_CALL ym_value_typeof(const ym_value* value) YM_NOEXCEPT;

YM_API ym_status YM_CALL ym_value_string(const ym_value* value, ym_string_view* out,
    ym_error* error) YM_NOEXCEPT;

YM_API ym_status YM_CALL ym_value_int64(const ym_value* value, int64_t* out,
    ym_error* error) YM_NOEXCEPT;

YM_API ym_status YM_CALL ym_value_uint64(const ym_value* value, uint64_t* out,
    ym_error* error) YM_NOEXCEPT;

YM_API ym_status YM_CALL ym_value_double(const ym_value* value, double* out,
    ym_error* error) YM_NOEXCEPT;

YM_API ym_status YM_CALL ym_value_bool(const ym_value* value, uint32_t* out,
    ym_error* error) YM_NOEXCEPT;

/** Array/object element count. Scalar input returns YM_SCHEMA_ERROR. */
YM_API ym_status YM_CALL ym_value_size(const ym_value* value, size_t* out,
    ym_error* error) YM_NOEXCEPT;

/** Missing key/index returns YM_NOT_FOUND and sets *out to NULL. */
YM_API ym_status YM_CALL ym_value_member(const ym_value* value, ym_string_view key,
    const ym_value** out, ym_error* error) YM_NOEXCEPT;

YM_API ym_status YM_CALL ym_value_at(const ym_value* value, size_t index, const ym_value** out,
    ym_error* error) YM_NOEXCEPT;

#ifdef __cplusplus
}
#endif

#endif
