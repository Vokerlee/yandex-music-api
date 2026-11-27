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

#ifndef YANDEX_MUSIC_ACCOUNT_H
#define YANDEX_MUSIC_ACCOUNT_H

#include <yandex_music/api.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t ym_library_type;

enum {
    YM_LIBRARY_TRACK = 0,
    YM_LIBRARY_ARTIST = 1,
    YM_LIBRARY_ALBUM = 2,
    YM_LIBRARY_PLAYLIST = 3,
};

/** Includes account UID and subscription details. */
YM_API ym_status YM_CALL ym_account_status(ym_client* client, const ym_request_options* options,
    ym_request** out, ym_error* error) YM_NOEXCEPT;

YM_API ym_status YM_CALL ym_likes(ym_client* client, ym_string_view user_id, ym_library_type type,
    const ym_request_options* options, ym_request** out, ym_error* error) YM_NOEXCEPT;

/** IDs are comma-separated; playlist IDs use owner:kind. liked is zero or one. */
YM_API ym_status YM_CALL ym_set_like(ym_client* client, ym_string_view user_id,
    ym_library_type type, ym_string_view ids, uint32_t liked, const ym_request_options* options,
    ym_request** out, ym_error* error) YM_NOEXCEPT;

#ifdef __cplusplus
}
#endif

#endif
