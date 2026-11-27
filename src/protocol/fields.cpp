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

#include "encoding_error.hpp"
#include "fields.hpp"
#include "percent.hpp"

#include <charconv>
#include <cmath>
#include <type_traits>

namespace ym::detail::wire {

std::expected<std::string, ym_error> encode_fields(std::span<const RequestField> fields, bool form,
    size_t limit) noexcept
{
    if (!limit)
        return encoding_failure(YM_INVALID_ARGUMENT, "Byte limit must be positive");
    if (fields.size() > 4096)
        return encoding_failure(YM_RESOURCE_LIMIT, "Too many request fields");
    try {
        std::string result;
        for (const auto& field : fields)
            if (field.value) {
                auto text = std::visit(
                    [&](const auto& item) -> std::expected<std::string, ym_error> {
                        using T = std::decay_t<decltype(item)>;
                        if constexpr (std::is_same_v<T, std::string>) {
                            if (item.size() > limit)
                                return encoding_failure(YM_RESOURCE_LIMIT,
                                    "Field exceeds byte limit");
                            return item;
                        } else if constexpr (std::is_same_v<T, bool>)
                            return std::string(item ? "true" : "false");
                        else if constexpr (std::is_arithmetic_v<T>) {
                            if constexpr (std::is_floating_point_v<T>)
                                if (!std::isfinite(item))
                                    return encoding_failure(YM_INVALID_ARGUMENT,
                                        "Non-finite request number");
                            char buffer[128];
                            auto converted = std::to_chars(buffer, buffer + sizeof(buffer), item);
                            if (converted.ec != std::errc{})
                                return encoding_failure(YM_INVALID_ARGUMENT,
                                    "Request number out of range");
                            return std::string(buffer, converted.ptr);
                        } else
                            return encoding_failure(YM_INVALID_ARGUMENT,
                                "Null/nested form values require explicit endpoint encoding");
                    },
                    field.value->data
                );
                if (!text)
                    return std::unexpected(text.error());
                auto key = percent_encode(field.key, form, limit),
                     value = percent_encode(*text, form, limit);
                if (!key)
                    return std::unexpected(key.error());
                if (!value)
                    return std::unexpected(value.error());
                if ((!result.empty() && !append_bounded(result, "&", limit)) ||
                    !append_bounded(result, *key, limit) || !append_bounded(result, "=", limit) ||
                    !append_bounded(result, *value, limit))
                    return encoding_failure(YM_RESOURCE_LIMIT, "Encoded fields exceed byte limit");
            }
        return result;
    } catch (const std::bad_alloc&) {
        return encoding_failure(YM_OUT_OF_MEMORY, "Field encoding allocation failed");
    } catch (...) {
        return encoding_failure(YM_INVALID_ARGUMENT, "Fields could not be encoded");
    }
}

} // namespace ym::detail::wire
