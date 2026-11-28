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

#pragma once

#include <string_view>

#include <yandex_music/download.h>
#include <yandex_music/error.hpp>

namespace ym {

[[nodiscard]] inline Result<void> download_file(std::string_view url, std::string_view filename,
    uint64_t timeout_ms = 30000, ym_download_progress_fn progress = nullptr,
    void* userdata = nullptr) noexcept
{
    ym_error failure;
    if (ym_file_download({url.data(), url.size()}, {filename.data(), filename.size()}, timeout_ms,
            progress, userdata, &failure) != YM_OK)
        return std::unexpected(Error{failure});
    return {};
}

} // namespace ym
