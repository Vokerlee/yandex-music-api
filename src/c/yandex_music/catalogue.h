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

#ifndef YANDEX_MUSIC_CATALOGUE_H
#define YANDEX_MUSIC_CATALOGUE_H

#include <yandex_music/api.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Comma-separated IDs, optionally track:album pairs; preserves returned order and fields. */
YM_API ym_status YM_CALL ym_tracks(ym_client* client, ym_string_view ids,
    const ym_request_options* options, ym_request** out, ym_error* error) YM_NOEXCEPT;

YM_API ym_status YM_CALL ym_track_similar(ym_client* client, ym_string_view track_id,
    const ym_request_options* options, ym_request** out, ym_error* error) YM_NOEXCEPT;

YM_API ym_status YM_CALL ym_track_supplement(ym_client* client, ym_string_view track_id,
    const ym_request_options* options, ym_request** out, ym_error* error) YM_NOEXCEPT;

#ifdef __cplusplus
}
#endif

#endif
