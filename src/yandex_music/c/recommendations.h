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

#ifndef YANDEX_MUSIC_RECOMMENDATIONS_H
#define YANDEX_MUSIC_RECOMMENDATIONS_H

#include <yandex_music/c/api.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Personalized feed for the authenticated account. */
YM_API ym_status YM_CALL ym_recommendations(ym_client* client, const ym_request_options* options,
    ym_request** out, ym_error* error) YM_NOEXCEPT;

/** Known IDs include daily, missedLikes, recentTracks and neverHeard. */
YM_API ym_status YM_CALL ym_personal_playlist(ym_client* client, ym_string_view playlist_id,
    const ym_request_options* options, ym_request** out, ym_error* error) YM_NOEXCEPT;

#ifdef __cplusplus
}
#endif

#endif
