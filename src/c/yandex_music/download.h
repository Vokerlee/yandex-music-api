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

/** Return nonzero to cancel. total_bytes may be zero when unknown.
 * Callbacks must not throw. */
typedef int(YM_CALL* ym_download_progress_fn)(uint64_t received_bytes, uint64_t total_bytes,
    void* userdata);

/** Blocking HTTPS download, without OAuth headers. Streams to filename.part;
 * replaces filename only after success, and removes partial output on failure.
 * A pre-existing .part file is left untouched and reports YM_IO_ERROR. */
YM_API ym_status YM_CALL ym_file_download(ym_string_view url, ym_string_view filename,
    uint64_t timeout_ms, ym_download_progress_fn progress, void* userdata,
    ym_error* error) YM_NOEXCEPT;

/** Obtains a raw file URL, then saves the audio. Each phase uses timeout_ms.
 * The chosen codec/container is preserved. Encrypted transports are unsupported. */
YM_API ym_status YM_CALL ym_track_download(ym_client* client, ym_string_view track_id,
    ym_string_view filename, ym_string_view quality, ym_string_view codecs, uint64_t timeout_ms,
    ym_download_progress_fn progress, void* userdata, ym_error* error) YM_NOEXCEPT;

#ifdef __cplusplus
}
#endif

#endif
