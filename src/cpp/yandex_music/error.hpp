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
#include <expected>
#include <string_view>
#include <yandex_music/error.h>

namespace ym
{
class error
{
    ym_error value_ { };

public:
    error() noexcept { ym_error_init(&value_); }
    explicit error(ym_error value) noexcept
        : value_(value)
    {
    }
    [[nodiscard]] ym_status code() const noexcept { return value_.code; }
    [[nodiscard]] std::string_view message() const noexcept { return value_.message; }
    [[nodiscard]] const ym_error& native() const noexcept { return value_; }
};
template <class T> using result = std::expected<T, error>;
} // namespace ym
