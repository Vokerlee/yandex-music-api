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

#include <yandex_music/c/yandex_music.h>
#include <yandex_music/cpp/client.hpp>
#include <yandex_music/cpp/download.hpp>
#include <yandex_music/cpp/error.hpp>
#include <yandex_music/cpp/ids.hpp>
#include <yandex_music/cpp/request.hpp>
#include <yandex_music/cpp/response.hpp>
#include <yandex_music/cpp/result.hpp>

namespace ym {

[[nodiscard]] inline std::string_view version() noexcept
{
    return ym_version();
}

[[nodiscard]] inline std::string_view build_info() noexcept
{
    return ym_build_info();
}

} // namespace ym
