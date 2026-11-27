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
#include "json_write.hpp"

#include <cmath>
#include <type_traits>

#include <nlohmann/json.hpp>

namespace ym::detail::wire {
namespace {

struct Failure {
    ym_status code;
    const char* message;
};

struct Writer {
    size_t limit, bytes = 0, nodes = 0;

    void text(std::string_view value)
    {
        if (!valid_utf8(value))
            throw Failure{YM_INVALID_ARGUMENT, "Invalid JSON UTF-8"};
        if (value.size() > limit - bytes)
            throw Failure{YM_RESOURCE_LIMIT, "JSON text exceeds byte limit"};
        bytes += value.size();
    }

    nlohmann::ordered_json map(const ym_value& value, size_t depth = 0)
    {
        if (++nodes > 4096 || depth > 64)
            throw Failure{YM_RESOURCE_LIMIT, "JSON node/depth limit exceeded"};
        return std::visit(
            [&](const auto& item) -> nlohmann::ordered_json {
                using T = std::decay_t<decltype(item)>;
                if constexpr (std::is_same_v<T, std::monostate>)
                    return nullptr;
                else if constexpr (std::is_same_v<T, std::string>) {
                    text(item);
                    return item;
                } else if constexpr (std::is_same_v<T, ym_value::Array>) {
                    auto result = nlohmann::ordered_json::array();
                    for (const auto& child : item)
                        result.push_back(map(child, depth + 1));
                    return result;
                } else if constexpr (std::is_same_v<T, ym_value::Object>) {
                    auto result = nlohmann::ordered_json::object();
                    for (const auto& [key, child] : item) {
                        text(key);
                        if (result.contains(key))
                            throw Failure{YM_INVALID_ARGUMENT, "Duplicate JSON key"};
                        result[key] = map(child, depth + 1);
                    }
                    return result;
                } else {
                    if constexpr (std::is_floating_point_v<T>)
                        if (!std::isfinite(item))
                            throw Failure{YM_INVALID_ARGUMENT, "Non-finite JSON number"};
                    return item;
                }
            },
            value.data
        );
    }
};

} // namespace

std::expected<std::string, ym_error> encode_json(const ym_value& input, size_t limit) noexcept
{
    if (!limit)
        return encoding_failure(YM_INVALID_ARGUMENT, "Byte limit must be positive");
    try {
        Writer write{limit};
        auto result = write.map(input).dump();
        if (result.size() > limit)
            return encoding_failure(YM_RESOURCE_LIMIT, "JSON output exceeds byte limit");
        return result;
    } catch (const Failure& error) {
        return encoding_failure(error.code, error.message);
    } catch (const std::bad_alloc&) {
        return encoding_failure(YM_OUT_OF_MEMORY, "JSON encoding allocation failed");
    } catch (...) {
        return encoding_failure(YM_INVALID_ARGUMENT, "JSON could not be encoded");
    }
}

} // namespace ym::detail::wire
