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

#include <yandex_music/client.hpp>
#include <yandex_music/download.hpp>
#include <yandex_music/error.hpp>
#include <yandex_music/ids.hpp>
#include <yandex_music/request.hpp>
#include <yandex_music/response.hpp>
#include <yandex_music/result.hpp>
#include <yandex_music/yandex_music.h>

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
