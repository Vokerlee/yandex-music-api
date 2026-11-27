/*
 * Endpoint behavior references MarshalX/yandex-music-api at
 * 9ca132e5c18a72aab779c1fc68c169380a9357d0:
 * yandex_music/_client/tracks.py (download variants and raw file-info).
 * Copyright (C) 2019-2026 Ilya (Marshal) <https://github.com/MarshalX>
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

#include "core/api.hpp"
#include "file_signature.hpp"

#include <chrono>

#include <yandex_music/download.h>

ym_status YM_CALL ym_track_download_info(ym_client* client, ym_string_view id,
    const ym_request_options* options, ym_request** out, ym_error* error) YM_NOEXCEPT
{
    return ym::detail::start_api(
        client,
        [&](auto& spec) { spec.path = {"tracks", ym::detail::identifier(id), "download-info"}; },
        options, out, error
    );
}

ym_status YM_CALL ym_track_file_info(ym_client* client, ym_string_view input_id,
    ym_string_view input_quality, ym_string_view input_codecs, const ym_request_options* options,
    ym_request** out, ym_error* error) YM_NOEXCEPT
{
    return ym::detail::start_api(
        client,
        [&](auto& spec) {
            auto id = ym::detail::identifier(input_id);
            auto quality = ym::detail::identifier(input_quality);
            auto codecs = ym::detail::identifier(input_codecs);
            const auto now = std::chrono::system_clock::now().time_since_epoch();
            auto timestamp =
                std::to_string(std::chrono::duration_cast<std::chrono::seconds>(now).count());
            auto signature = ym::detail::file_signature(timestamp, id, quality, codecs);
            spec.path = {"get-file-info"};
            spec.query = {ym::detail::field("ts", std::move(timestamp)),
                ym::detail::field("trackId", std::move(id)),
                ym::detail::field("quality", std::move(quality)),
                ym::detail::field("codecs", std::move(codecs)),
                ym::detail::field("transports", "raw"),
                ym::detail::field("sign", std::move(signature))};
            spec.headers.emplace_back("X-Yandex-Music-Client",
                "YandexMusicDesktopAppWindows/5.95.0");
        },
        options, out, error
    );
}
