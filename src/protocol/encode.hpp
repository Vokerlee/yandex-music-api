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
#include "core/engine.hpp"
#include <expected>
#include <optional>
#include <span>
namespace ym::detail::wire
{
enum class method { get, post, put, remove, patch, head };
enum class body_kind { none, form, json };
/** absent means omitted; ym_value{} means explicit JSON null. */
struct field {
    std::string key;
    std::optional<ym_value> value;
};
struct request_spec {
    std::string origin { };
    method verb = method::get;
    std::vector<std::string> path { };
    std::vector<field> query { };
    body_kind encoding = body_kind::none;
    std::vector<field> body { };
    std::optional<ym_value> json_root { };
    std::vector<std::pair<std::string, std::string>> headers { };
    bool mutating = false;
};
/** Pure serialization: no authentication defaults, network calls or HTTP-verb retry guesses.
 * Text forms reject null/nested values; an endpoint must choose its exact text representation.
 * Input and output byte counts, node counts and nesting are bounded. */
std::expected<request_input, ym_error> encode(
    const request_spec&, size_t max_bytes = 1024 * 1024) noexcept;
} // namespace ym::detail::wire
