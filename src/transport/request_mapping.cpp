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

#include "curl_transfer.hpp"

#include <algorithm>
#include <array>
#include <limits>
#include <new>
#include <string_view>

namespace ym::detail {
namespace {

bool token_character(unsigned char c) noexcept
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') ||
        std::string_view("!#$%&'*+-.^_`|~").find(static_cast<char>(c)) != std::string_view::npos;
}

bool clean_value(const std::string& value) noexcept
{
    return std::all_of(value.begin(), value.end(),
        [](unsigned char c) { return c >= 32 && c != 127; });
}

void configure_headers(CurlTransfer& transfer, const RequestInput& input)
{
    for (const auto& [name, value] : input.headers) {
        if (name.empty() || !std::all_of(name.begin(), name.end(), token_character) ||
            !clean_value(value))
            throw std::invalid_argument("Malformed HTTP header");
        // In curl, a semicolon sends an empty field; a colon removes a default.
        const auto line = name + (value.empty() ? ";" : ": " + value);
        auto* next = curl_slist_append(transfer.headers, line.c_str());
        if (!next)
            throw std::bad_alloc();
        transfer.headers = next;
    }
}

} // namespace

void configure_request(CurlTransfer& transfer, const RequestInput& input)
{
    const auto& method = input.method;
    constexpr std::array<std::string_view, 6> methods{"GET", "HEAD", "POST", "PUT", "PATCH",
        "DELETE"};
    if (std::find(methods.begin(), methods.end(), method) == methods.end())
        throw std::invalid_argument("Unsupported HTTP method");
    if ((method == "GET" || method == "HEAD") && !input.body.empty())
        throw std::invalid_argument("Read methods cannot carry a body");
    if (input.body.size() > static_cast<uint64_t>(std::numeric_limits<curl_off_t>::max()))
        throw std::invalid_argument("Body exceeds curl size range");
    configure_headers(transfer, input);

    auto* handle = transfer.handle;
    curl_option(handle, CURLOPT_HTTPHEADER, transfer.headers);
    if (method == "HEAD")
        curl_option(handle, CURLOPT_NOBODY, 1L);
    else if (method != "GET") {
        curl_option(handle, CURLOPT_POSTFIELDSIZE_LARGE,
            static_cast<curl_off_t>(input.body.size()));
        // The engine retains input until completion or synchronous detachment.
        curl_option(handle, CURLOPT_POSTFIELDS, input.body.data());
    }
    curl_option(handle, CURLOPT_CUSTOMREQUEST, method.c_str());
}

} // namespace ym::detail
