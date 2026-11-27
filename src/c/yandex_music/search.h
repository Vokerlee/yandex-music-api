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

#ifndef YANDEX_MUSIC_SEARCH_H
#define YANDEX_MUSIC_SEARCH_H

#include <yandex_music/api.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t ym_search_type;

enum {
    YM_SEARCH_ALL = 0,
    YM_SEARCH_TRACK = 1,
    YM_SEARCH_ARTIST = 2,
    YM_SEARCH_ALBUM = 3,
    YM_SEARCH_PLAYLIST = 4,
    YM_SEARCH_USER = 5,
    YM_SEARCH_PODCAST = 6,
    YM_SEARCH_PODCAST_EPISODE = 7,
};

/** Pages start at zero. Result categories contain total, perPage and results. */
YM_API ym_status YM_CALL ym_search(ym_client* client, ym_string_view text, ym_search_type type,
    uint32_t page, const ym_request_options* options, ym_request** out,
    ym_error* error) YM_NOEXCEPT;

YM_API ym_status YM_CALL ym_search_suggest(ym_client* client, ym_string_view text,
    const ym_request_options* options, ym_request** out, ym_error* error) YM_NOEXCEPT;

#ifdef __cplusplus
}
#endif

#endif
