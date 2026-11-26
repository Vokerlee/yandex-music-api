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

#include "core/text.hpp"
#include "encoding_error.hpp"
#include "percent.hpp"

#include <new>

namespace ym::detail::wire {

std::expected<std::string, ym_error> percent_encode(std::string_view text, bool form,
    size_t limit) noexcept
{
    if (!limit)
        return encoding_failure(YM_INVALID_ARGUMENT, "Byte limit must be positive");
    if (!valid_utf8(text) || text.find('\0') != std::string_view::npos)
        return encoding_failure(YM_INVALID_ARGUMENT, "Invalid UTF-8 or NUL in request text");
    if (text.size() > limit)
        return encoding_failure(YM_RESOURCE_LIMIT, "Request text exceeds byte limit");
    try {
        std::string result;
        constexpr char hex[] = "0123456789ABCDEF";
        for (unsigned char c : text) {
            if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') ||
                c == '-' || c == '_' || c == '.' || c == '~') {
                const char byte = static_cast<char>(c);
                if (!append_bounded(result, {&byte, 1}, limit))
                    return encoding_failure(YM_RESOURCE_LIMIT, "Encoded text exceeds byte limit");
            } else if (form && c == ' ') {
                if (!append_bounded(result, "+", limit))
                    return encoding_failure(YM_RESOURCE_LIMIT, "Encoded text exceeds byte limit");
            } else {
                const char escaped[] = {'%', hex[c >> 4], hex[c & 15]};
                if (!append_bounded(result, {escaped, 3}, limit))
                    return encoding_failure(YM_RESOURCE_LIMIT, "Encoded text exceeds byte limit");
            }
        }
        return result;
    } catch (const std::bad_alloc&) {
        return encoding_failure(YM_OUT_OF_MEMORY, "Percent encoding allocation failed");
    } catch (...) {
        return encoding_failure(YM_INVALID_ARGUMENT, "Request text could not be encoded");
    }
}

} // namespace ym::detail::wire
