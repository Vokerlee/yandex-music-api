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

#include "encode.hpp"
#include "core/error.hpp"
#include "core/text.hpp"
#include <algorithm>
#include <charconv>
#include <cmath>
#include <new>
#include <nlohmann/json.hpp>
#include <set>
#include <type_traits>

namespace ym::detail::wire
{
namespace
{
struct encoding_error {
    ym_status code;
    const char* message;
};
struct encoder {
    size_t maximum;
    size_t raw_bytes = 0;
    size_t nodes = 0;
    void consume(size_t size)
    {
        if (size > maximum - raw_bytes)
            throw encoding_error { YM_RESOURCE_LIMIT, "Request input exceeds byte limit" };
        raw_bytes += size;
    }
    void text(std::string_view value, bool allow_nul = false)
    {
        if (!valid_utf8(value) || (!allow_nul && value.find('\0') != std::string_view::npos))
            throw encoding_error { YM_INVALID_ARGUMENT, "Invalid UTF-8 or NUL in request text" };
        consume(value.size());
    }
    void append(std::string& output, std::string_view value)
    {
        if (value.size() > maximum - output.size())
            throw encoding_error { YM_RESOURCE_LIMIT, "Encoded request exceeds byte limit" };
        output.append(value);
    }
    std::string percent(std::string_view value, bool form)
    {
        text(value);
        std::string output;
        constexpr char hex[] = "0123456789ABCDEF";
        for (unsigned char c : value) {
            if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9')
                || c == '-' || c == '_' || c == '.' || c == '~') {
                const char byte = static_cast<char>(c);
                append(output, { &byte, 1 });
            } else if (form && c == ' ')
                append(output, "+");
            else {
                const char escape[] = { '%', hex[c >> 4], hex[c & 15] };
                append(output, { escape, 3 });
            }
        }
        return output;
    }
    void node()
    {
        if (++nodes > 4096)
            throw encoding_error { YM_RESOURCE_LIMIT, "Request node limit exceeded" };
    }
    std::string scalar(const ym_value& value)
    {
        node();
        return std::visit(
            [&](const auto& item) -> std::string {
                using T = std::decay_t<decltype(item)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    if (item.size() > maximum - raw_bytes)
                        throw encoding_error { YM_RESOURCE_LIMIT,
                            "Request input exceeds byte limit" };
                    return item;
                } else if constexpr (std::is_same_v<T, bool>)
                    return item ? "true" : "false";
                else if constexpr (std::is_arithmetic_v<T>) {
                    if constexpr (std::is_floating_point_v<T>)
                        if (!std::isfinite(item))
                            throw encoding_error { YM_INVALID_ARGUMENT,
                                "Non-finite request number" };
                    char buffer[128];
                    const auto converted = std::to_chars(buffer, buffer + sizeof(buffer), item);
                    if (converted.ec != std::errc { })
                        throw encoding_error { YM_INVALID_ARGUMENT,
                            "Request number is out of range" };
                    return { buffer, converted.ptr };
                } else
                    throw encoding_error { YM_INVALID_ARGUMENT,
                        "Null or nested form/query value needs explicit endpoint encoding" };
            },
            value.data);
    }
    nlohmann::ordered_json json(const ym_value& value, size_t depth = 0)
    {
        node();
        if (depth > 64)
            throw encoding_error { YM_RESOURCE_LIMIT, "Request nesting exceeds limit" };
        return std::visit(
            [&](const auto& item) -> nlohmann::ordered_json {
                using T = std::decay_t<decltype(item)>;
                if constexpr (std::is_same_v<T, std::monostate>)
                    return nullptr;
                else if constexpr (std::is_same_v<T, std::string>) {
                    text(item, true);
                    return item;
                } else if constexpr (std::is_same_v<T, ym_value::array>) {
                    auto output = nlohmann::ordered_json::array();
                    for (const auto& child : item)
                        output.push_back(json(child, depth + 1));
                    return output;
                } else if constexpr (std::is_same_v<T, ym_value::object>) {
                    auto output = nlohmann::ordered_json::object();
                    for (const auto& [key, child] : item) {
                        text(key, true);
                        if (output.contains(key))
                            throw encoding_error { YM_INVALID_ARGUMENT, "Duplicate JSON key" };
                        output[key] = json(child, depth + 1);
                    }
                    return output;
                } else {
                    if constexpr (std::is_floating_point_v<T>)
                        if (!std::isfinite(item))
                            throw encoding_error { YM_INVALID_ARGUMENT, "Non-finite JSON number" };
                    return item;
                }
            },
            value.data);
    }
    std::string fields(std::span<const field> fields, bool form)
    {
        std::string output;
        for (const auto& field : fields)
            if (field.value) {
                if (!output.empty())
                    append(output, "&");
                append(output, percent(field.key, form));
                append(output, "=");
                append(output, percent(scalar(*field.value), form));
            }
        return output;
    }
};
const char* method_name(method value)
{
    switch (value) {
    case method::get:
        return "GET";
    case method::post:
        return "POST";
    case method::put:
        return "PUT";
    case method::remove:
        return "DELETE";
    case method::patch:
        return "PATCH";
    case method::head:
        return "HEAD";
    }
    throw encoding_error { YM_INVALID_ARGUMENT, "Invalid HTTP method" };
}
} // namespace

std::expected<request_input, ym_error> encode(const request_spec& spec, size_t max_bytes) noexcept
{
    try {
        if (!max_bytes)
            throw encoding_error { YM_INVALID_ARGUMENT, "Request byte limit must be positive" };
        encoder encode { max_bytes };
        encode.text(spec.origin);
        if (!spec.origin.starts_with("https://") || spec.origin.size() == 8)
            throw encoding_error { YM_INVALID_ARGUMENT, "Request origin must use HTTPS" };
        const auto authority = std::string_view(spec.origin).substr(8);
        for (unsigned char c : authority)
            if (c <= 32 || c >= 127
                || std::string_view("/?#@\\").find(static_cast<char>(c)) != std::string_view::npos)
                throw encoding_error { YM_INVALID_ARGUMENT,
                    "Origin contains a path, credentials or invalid authority" };
        if (spec.path.size() > 64 || spec.query.size() > 4096 || spec.body.size() > 4096
            || spec.headers.size() > 256)
            throw encoding_error { YM_RESOURCE_LIMIT, "Too many request fields or path segments" };
        request_input output;
        output.method = method_name(spec.verb);
        output.mutating = spec.mutating;
        encode.append(output.url, spec.origin);
        if (spec.path.empty())
            encode.append(output.url, "/");
        for (const auto& segment : spec.path) {
            encode.append(output.url, "/");
            encode.append(output.url, encode.percent(segment, false));
        }
        const auto query = encode.fields(spec.query, false);
        if (!query.empty()) {
            encode.append(output.url, "?");
            encode.append(output.url, query);
        }
        std::set<std::string> header_names;
        auto header = [&](std::string name, std::string value) {
            encode.text(name);
            encode.text(value);
            if (name.empty() || value.find_first_of("\r\n") != std::string::npos)
                throw encoding_error { YM_INVALID_ARGUMENT, "Invalid header name or value" };
            for (unsigned char c : name)
                if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9')
                        || std::string_view("!#$%&'*+-.^_`|~").find(static_cast<char>(c))
                            != std::string_view::npos))
                    throw encoding_error { YM_INVALID_ARGUMENT, "Invalid header name" };
            auto lower = name;
            for (char& c : lower)
                if (c >= 'A' && c <= 'Z')
                    c = static_cast<char>(c + 'a' - 'A');
            if (!header_names.insert(lower).second)
                throw encoding_error { YM_INVALID_ARGUMENT, "Duplicate request header" };
            output.headers.emplace_back(std::move(name), std::move(value));
        };
        for (const auto& [name, value] : spec.headers)
            header(name, value);
        if (spec.encoding == body_kind::form) {
            if (spec.json_root)
                throw encoding_error { YM_INVALID_ARGUMENT, "JSON root supplied for a form body" };
            output.body = encode.fields(spec.body, true);
            header("Content-Type", "application/x-www-form-urlencoded");
        } else if (spec.encoding == body_kind::json) {
            nlohmann::ordered_json body;
            if (spec.json_root) {
                if (!spec.body.empty())
                    throw encoding_error { YM_INVALID_ARGUMENT, "Two JSON body sources supplied" };
                body = encode.json(*spec.json_root);
            } else {
                body = nlohmann::ordered_json::object();
                for (const auto& field : spec.body)
                    if (field.value) {
                        encode.text(field.key, true);
                        if (body.contains(field.key))
                            throw encoding_error { YM_INVALID_ARGUMENT, "Duplicate JSON key" };
                        body[field.key] = encode.json(*field.value);
                    }
            }
            output.body = body.dump();
            header("Content-Type", "application/json");
        } else if (spec.encoding != body_kind::none || !spec.body.empty() || spec.json_root)
            throw encoding_error { YM_INVALID_ARGUMENT,
                "Body data supplied without a valid encoding" };
        size_t total = output.url.size();
        auto count = [&](size_t amount) {
            if (amount > max_bytes - total)
                throw encoding_error { YM_RESOURCE_LIMIT, "Encoded request exceeds byte limit" };
            total += amount;
        };
        count(output.method.size());
        count(output.body.size());
        for (const auto& [name, value] : output.headers) {
            count(name.size());
            count(value.size());
        }
        return output;
    } catch (const encoding_error& failure) {
        ym_error error;
        fail(&error, failure.code, failure.message);
        return std::unexpected(error);
    } catch (const std::bad_alloc&) {
        ym_error error;
        fail(&error, YM_OUT_OF_MEMORY, "Request encoding allocation failed");
        return std::unexpected(error);
    } catch (...) {
        ym_error error;
        fail(&error, YM_INVALID_ARGUMENT, "Request could not be encoded");
        return std::unexpected(error);
    }
}
} // namespace ym::detail::wire
