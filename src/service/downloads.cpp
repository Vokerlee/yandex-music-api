/*
 * Endpoint behavior references MarshalX/yandex-music-api at
 * 9ca132e5c18a72aab779c1fc68c169380a9357d0:
 * yandex_music/_client/tracks.py and track/file_download_info.py (track download).
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
#include "download_file.hpp"

#include <memory>

#include <yandex_music/download.h>

ym_status YM_CALL ym_file_download(ym_string_view url, ym_string_view filename, uint64_t timeout_ms,
    ym_download_progress_fn progress, void* userdata, ym_error* error) YM_NOEXCEPT
{
    if ((!url.data && url.size) || (!filename.data && filename.size))
        return ym::detail::fail(error, YM_INVALID_ARGUMENT, "Download text data is required");
    return ym::detail::download_file(url.size ? std::string_view{url.data, url.size}
                                              : std::string_view{},
        filename.size ? std::string_view{filename.data, filename.size} : std::string_view{},
        timeout_ms, progress, userdata, error);
}

ym_status YM_CALL ym_track_download(ym_client* client, ym_string_view id, ym_string_view filename,
    ym_string_view quality, ym_string_view codecs, uint64_t timeout_ms,
    ym_download_progress_fn progress, void* userdata, ym_error* error) YM_NOEXCEPT
{
    if (!filename.data || !filename.size || !timeout_ms)
        return ym::detail::fail(error, YM_INVALID_ARGUMENT,
            "Filename and a positive timeout are required");
    ym_request* raw_request = nullptr;
    auto status = ym_track_file_info(client, id, quality, codecs, nullptr, &raw_request, error);
    if (status != YM_OK)
        return status;
    std::unique_ptr<ym_request, decltype(&ym_request_destroy)> request{raw_request,
        ym_request_destroy};
    ym_result* raw_result = nullptr;
    status = ym_client_wait(client, request.get(), timeout_ms, &raw_result, error);
    if (status != YM_OK)
        return status;
    std::unique_ptr<ym_result, decltype(&ym_result_destroy)> result{raw_result, ym_result_destroy};

    const ym_value* info = nullptr;
    const ym_value* transport = nullptr;
    const ym_value* url = nullptr;
    ym_string_view transport_text{};
    ym_string_view url_text{};
    if (ym_value_member(ym_result_root(result.get()), {"downloadInfo", 12}, &info, nullptr) !=
            YM_OK ||
        ym_value_member(info, {"transport", 9}, &transport, nullptr) != YM_OK ||
        ym_value_string(transport, &transport_text, nullptr) != YM_OK ||
        ym_value_member(info, {"url", 3}, &url, nullptr) != YM_OK ||
        ym_value_string(url, &url_text, nullptr) != YM_OK)
        return ym::detail::fail(error, YM_SCHEMA_ERROR, "Download info requires transport and URL");
    if (std::string_view{transport_text.data, transport_text.size} != "raw")
        return ym::detail::fail(error, YM_UNSUPPORTED,
            "Download requires an unencrypted raw transport");
    return ym_file_download(url_text, filename, timeout_ms, progress, userdata, error);
}
