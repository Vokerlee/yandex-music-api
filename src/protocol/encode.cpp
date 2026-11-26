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
#include "json_write.hpp"
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
        if (spec.encoding == BodyKind::form) {
            if (spec.json_root)
                return encoding_failure(YM_INVALID_ARGUMENT, "JSON supplied for form body");
            auto body = encode_fields(spec.body, true, limit);
            if (!body)
                return std::unexpected(body.error());
            output.body = std::move(*body);
            output.headers.emplace_back("Content-Type", "application/x-www-form-urlencoded");
        } else if (spec.encoding == BodyKind::json) {
            if (spec.json_root && !spec.body.empty())
                return encoding_failure(YM_INVALID_ARGUMENT, "Two JSON body sources supplied");
            ym_value value;
            if (spec.json_root)
                value = *spec.json_root;
            else {
                ym_value::Object object;
                for (const auto& field : spec.body)
                    if (field.value)
                        object.emplace_back(field.key, *field.value);
                value = ym_value(std::move(object));
            }
            auto body = encode_json(value, limit);
            if (!body)
                return std::unexpected(body.error());
            output.body = std::move(*body);
            output.headers.emplace_back("Content-Type", "application/json");
        } else if (spec.encoding != BodyKind::none || !spec.body.empty() || spec.json_root)
            return encoding_failure(YM_INVALID_ARGUMENT,
                "Body data supplied without matching encoding");
        if (spec.headers.size() > 256)
            return encoding_failure(YM_RESOURCE_LIMIT, "Too many headers");
        std::set<std::string> names;
        if (!output.headers.empty())
            names.insert("content-type");
        for (const auto& [key, value] : spec.headers) {
            if (key.empty() || !valid_utf8(value) ||
                value.find_first_of("\r\n\0", 0, 3) != std::string::npos)
                return encoding_failure(YM_INVALID_ARGUMENT, "Invalid request header");
            std::string lower;
            for (unsigned char c : key) {
                if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') ||
                        std::string_view("!#$%&'*+-.^_`|~").find(static_cast<char>(c)) !=
                            std::string_view::npos))
                    return encoding_failure(YM_INVALID_ARGUMENT, "Invalid header name");
                lower.push_back(c >= 'A' && c <= 'Z' ? static_cast<char>(c + 'a' - 'A')
                                                     : static_cast<char>(c));
            }
            if (!names.insert(lower).second)
                return encoding_failure(YM_INVALID_ARGUMENT, "Duplicate header name");
            output.headers.emplace_back(key, value);
        }
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
