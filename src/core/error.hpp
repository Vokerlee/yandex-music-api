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
#include <algorithm>
#include <cstring>
#include <string_view>
#include <yandex_music/error.h>

namespace ym::detail
{
template <size_t N> void copy_text(char (&out)[N], std::string_view text) noexcept
{
    const auto length = std::min(text.size(), N - 1);
    if (length)
        std::memcpy(out, text.data(), length);
    out[length] = '\0';
}
inline ym_status fail(ym_error* out, ym_status code, std::string_view message) noexcept
{
    if (out) {
        ym_error_init(out);
        out->code = code;
        copy_text(out->message, message);
    }
    return code;
}
inline void clear(ym_error* out) noexcept
{
    if (out)
        ym_error_init(out);
}
} // namespace ym::detail
