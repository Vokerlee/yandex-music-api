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

#ifndef YANDEX_MUSIC_DOWNLOAD_H
#define YANDEX_MUSIC_DOWNLOAD_H

#include <yandex_music/api.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Lists available legacy codec/bitrate variants. */
YM_API ym_status YM_CALL ym_track_download_info(ym_client* client, ym_string_view track_id,
    const ym_request_options* options, ym_request** out, ym_error* error) YM_NOEXCEPT;

/** Requests signed raw file URLs; quality: nq, hq or lossless; codecs: comma-separated.
 * The response reports the actual quality and codec. Access depends on the account. */
YM_API ym_status YM_CALL ym_track_file_info(ym_client* client, ym_string_view track_id,
    ym_string_view quality, ym_string_view codecs, const ym_request_options* options,
    ym_request** out, ym_error* error) YM_NOEXCEPT;

#ifdef __cplusplus
}
#endif

#endif
