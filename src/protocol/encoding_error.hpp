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

#include "core/error.hpp"

#include <expected>
#include <string>

namespace ym::detail::wire {

inline std::unexpected<ym_error> encoding_failure(ym_status code, const char* message) noexcept
{
    ym_error error;
    fail(&error, code, message);
    return std::unexpected(error);
}

inline bool append_bounded(std::string& output, std::string_view text, size_t limit)
{
    if (output.size() > limit || text.size() > limit - output.size())
        return false;
    output.append(text);
    return true;
}

} // namespace ym::detail::wire
