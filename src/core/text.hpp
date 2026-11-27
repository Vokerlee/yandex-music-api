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

namespace ym::detail {

inline bool valid_utf8(std::string_view input) noexcept
{
    for (size_t i = 0; i < input.size();) {
        const auto c = static_cast<unsigned char>(input[i++]);
        if (c < 0x80)
            continue;
        unsigned count = 0;
        if (c >= 0xc2 && c <= 0xdf)
            count = 1;
        else if (c >= 0xe0 && c <= 0xef)
            count = 2;
        else if (c >= 0xf0 && c <= 0xf4)
            count = 3;
        else
            return false;
        if (input.size() - i < count)
            return false;
        const auto second = static_cast<unsigned char>(input[i]);
        if ((c == 0xe0 && second < 0xa0) || (c == 0xed && second >= 0xa0) ||
            (c == 0xf0 && second < 0x90) || (c == 0xf4 && second >= 0x90))
            return false;
        for (unsigned n = 0; n < count; ++n) {
            const auto continuation = static_cast<unsigned char>(input[i++]);
            if (continuation < 0x80 || continuation > 0xbf)
                return false;
        }
    }
    return true;
}

} // namespace ym::detail
