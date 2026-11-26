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

#ifndef YANDEX_MUSIC_RESPONSE_H
#define YANDEX_MUSIC_RESPONSE_H
#include <yandex_music/result.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t ym_response_envelope;
enum { YM_RESPONSE_AUTO = 0, YM_RESPONSE_RESULT = 1, YM_RESPONSE_ROOT = 2 };
typedef struct ym_response_options {
    size_t struct_size;
    uint32_t version;
    size_t max_bytes;
    size_t max_depth;
    size_t max_nodes;
    size_t max_collection_items;
    ym_response_envelope envelope;
} ym_response_options;
YM_API void YM_CALL ym_response_options_init(ym_response_options* options);
/** Offline response decoding; no HTTP. AUTO follows upstream: a non-null result
 * is unwrapped, otherwise the root object is retained. RESULT requires the key
 * and preserves explicit null; ROOT retains any JSON value. HTTP/service errors
 * fail with bounded detail. Unknown wire fields and their spelling are preserved. */
YM_API ym_status YM_CALL ym_response_parse(uint32_t http_status, ym_string_view body,
    const ym_response_options* options, ym_result** out, ym_error* error);
#ifdef __cplusplus
}
#endif
#endif
