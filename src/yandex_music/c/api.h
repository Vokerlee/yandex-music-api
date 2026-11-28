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

#ifndef YANDEX_MUSIC_API_H
#define YANDEX_MUSIC_API_H

#include <yandex_music/c/request.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ym_api_field {
    ym_string_view name;
    ym_string_view value;
} ym_api_field;

/** Paths are unescaped segments, query/form fields are unescaped UTF-8.
 * All inputs are copied before return. JSON and form bodies are mutually exclusive. */
typedef struct ym_api_call {
    ym_string_view method;
    const ym_string_view* path;
    size_t path_count;
    const ym_api_field* query;
    size_t query_count;
    const ym_api_field* form;
    size_t form_count;
    ym_string_view json;
} ym_api_call;

/** Queues a request to api.music.yandex.net using the client's OAuth token.
 * Completion and result ownership follow ym_request. No network I/O occurs here. */
YM_API ym_status YM_CALL ym_client_request(ym_client* client, const ym_api_call* call,
    const ym_request_options* options, ym_request** out, ym_error* error) YM_NOEXCEPT;

#ifdef __cplusplus
}
#endif

#endif
