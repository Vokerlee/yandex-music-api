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

#ifndef YANDEX_MUSIC_CLIENT_H
#define YANDEX_MUSIC_CLIENT_H

#include <yandex_music/error.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ym_client ym_client;

/** Append-only, size-versioned input. All complete fields are copied on create.
 * The minimum accepted prefix contains struct_size and version, including padding.
 * Fields omitted by an older caller retain defaults. Unknown tail bytes are ignored. */
typedef struct ym_client_options {
    size_t struct_size;
    uint32_t version;
    ym_string_view token;
    ym_string_view language;
    size_t max_response_bytes;
    size_t max_pending_requests;
} ym_client_options;

YM_API void YM_CALL ym_client_options_init(ym_client_options* options) YM_NOEXCEPT;

/** options may be NULL for defaults. On failure *out is NULL. No HTTP is performed. */
YM_API ym_status YM_CALL ym_client_create(const ym_client_options* options, ym_client** out,
    ym_error* error) YM_NOEXCEPT;

/** Local destruction; NULL is harmless. Calls on one client must be serialized. */
YM_API void YM_CALL ym_client_destroy(ym_client* client) YM_NOEXCEPT;

/** Borrowed until client destruction; NULL client returns an empty span. */
YM_API ym_string_view YM_CALL ym_client_language(const ym_client* client) YM_NOEXCEPT;

#ifdef __cplusplus
}
#endif

#endif
