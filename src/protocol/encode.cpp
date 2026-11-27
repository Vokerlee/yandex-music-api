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
#include "encode.hpp"
#include "encoding_error.hpp"
#include "percent.hpp"

#include <set>

namespace ym::detail::wire {
namespace {

const char* method_name(HttpMethod value) noexcept
{
    switch (value) {
    case HttpMethod::get:
        return "GET";
    case HttpMethod::post:
        return "POST";
    case HttpMethod::put:
        return "PUT";
    case HttpMethod::remove:
        return "DELETE";
    case HttpMethod::patch:
        return "PATCH";
    case HttpMethod::head:
        return "HEAD";
    }
    return nullptr;
}

} // namespace

std::expected<RequestInput, ym_error> encode(const RequestSpec& spec, size_t limit) noexcept
{
    if (!limit)
        return encoding_failure(YM_INVALID_ARGUMENT, "Byte limit must be positive");
    if (!valid_utf8(spec.origin) || !spec.origin.starts_with("https://") || spec.origin.size() == 8)
        return encoding_failure(YM_INVALID_ARGUMENT, "Origin must use HTTPS");
    for (unsigned char c : std::string_view(spec.origin).substr(8))
        if (c <= 32 || c >= 127 ||
            std::string_view("/?#@\\").find(static_cast<char>(c)) != std::string_view::npos)
            return encoding_failure(YM_INVALID_ARGUMENT,
                "Origin contains invalid authority or credentials");
    if (spec.path.size() > 64)
        return encoding_failure(YM_RESOURCE_LIMIT, "Too many path segments");
    const auto* verb = method_name(spec.verb);
    if (!verb)
        return encoding_failure(YM_INVALID_ARGUMENT, "Invalid HTTP method");
    try {
        RequestInput output;
        output.method = verb;
        output.mutating = spec.mutating;
        if (!append_bounded(output.url, spec.origin, limit))
            return encoding_failure(YM_RESOURCE_LIMIT, "URL exceeds byte limit");
        if (spec.path.empty() && !append_bounded(output.url, "/", limit))
            return encoding_failure(YM_RESOURCE_LIMIT, "URL exceeds byte limit");
        for (const auto& segment : spec.path) {
            auto escaped = percent_encode(segment, false, limit);
            if (!escaped)
                return std::unexpected(escaped.error());
            if (!append_bounded(output.url, "/", limit) ||
                !append_bounded(output.url, *escaped, limit))
                return encoding_failure(YM_RESOURCE_LIMIT, "URL exceeds byte limit");
        }
        auto query = encode_fields(spec.query, false, limit);
        if (!query)
            return std::unexpected(query.error());
        if (!query->empty() &&
            (!append_bounded(output.url, "?", limit) || !append_bounded(output.url, *query, limit)))
            return encoding_failure(YM_RESOURCE_LIMIT, "URL exceeds byte limit");
        size_t total = output.url.size();
        auto count = [&](size_t amount) {
            if (amount > limit - total)
                return false;
            total += amount;
            return true;
        };
        if (!count(output.method.size()) || !count(output.body.size()))
            return encoding_failure(YM_RESOURCE_LIMIT, "Encoded request exceeds byte limit");
        for (const auto& [key, value] : output.headers)
            if (!count(key.size()) || !count(value.size()))
                return encoding_failure(YM_RESOURCE_LIMIT, "Encoded request exceeds byte limit");
        return output;
    } catch (const std::bad_alloc&) {
        return encoding_failure(YM_OUT_OF_MEMORY, "Request encoding allocation failed");
    } catch (...) {
        return encoding_failure(YM_INVALID_ARGUMENT, "Request could not be encoded");
    }
}

} // namespace ym::detail::wire
