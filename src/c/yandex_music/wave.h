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

#ifndef YANDEX_MUSIC_WAVE_H
#define YANDEX_MUSIC_WAVE_H

#include <yandex_music/api.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Available activities and setting values, including serializedSeed for each choice.
 * seeds may be empty, or contain comma-separated current seeds. */
YM_API ym_status YM_CALL ym_wave_settings(ym_client* client, ym_string_view seeds,
    const ym_request_options* options, ym_request** out, ym_error* error) YM_NOEXCEPT;

/** Station names and IDs for genres, moods and activities, in the client's language. */
YM_API ym_status YM_CALL ym_wave_stations(ym_client* client, const ym_request_options* options,
    ym_request** out, ym_error* error) YM_NOEXCEPT;

/** seed examples: user:onyourwave, artist:42, track:123, genre:rock. */
YM_API ym_status YM_CALL ym_wave_start(ym_client* client, ym_string_view seed,
    const ym_request_options* options, ym_request** out, ym_error* error) YM_NOEXCEPT;

/** Combines station, genre, artist and setting seeds. All spans are copied. */
YM_API ym_status YM_CALL ym_wave_start_seeds(ym_client* client, const ym_string_view* seeds,
    size_t seed_count, const ym_request_options* options, ym_request** out,
    ym_error* error) YM_NOEXCEPT;

/** queue is a comma-separated list of already received track IDs (or track:album pairs). */
YM_API ym_status YM_CALL ym_wave_next(ym_client* client, ym_string_view session_id,
    ym_string_view queue, const ym_request_options* options, ym_request** out,
    ym_error* error) YM_NOEXCEPT;

/** Events include radioStarted, trackStarted, trackFinished, skip, like and dislike.
 * Track events require track_id; finish/skip/dislike also send played_seconds. */
YM_API ym_status YM_CALL ym_wave_feedback(ym_client* client, ym_string_view session_id,
    ym_string_view event, ym_string_view track_id, double played_seconds, ym_string_view batch_id,
    const ym_request_options* options, ym_request** out, ym_error* error) YM_NOEXCEPT;

#ifdef __cplusplus
}
#endif

#endif
