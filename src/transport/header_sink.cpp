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

#include "response_sink.hpp"

#include <algorithm>
#include <charconv>
#include <limits>
#include <new>
#include <string_view>

namespace ym::detail {
namespace {

bool equal_name(std::string_view a, std::string_view b) noexcept
{
    if (a.size() != b.size())
        return false;
    for (size_t i = 0; i < a.size(); ++i) {
        auto c = a[i];
        if (c >= 'A' && c <= 'Z')
            c = static_cast<char>(c - 'A' + 'a');
        if (c != b[i])
            return false;
    }
    return true;
}

std::string_view trim(std::string_view s) noexcept
{
    while (!s.empty() && (s.front() == ' ' || s.front() == '\t'))
        s.remove_prefix(1);
    while (
        !s.empty() && (s.back() == ' ' || s.back() == '\t' || s.back() == '\r' || s.back() == '\n'))
        s.remove_suffix(1);
    return s;
}

void read_metadata(ResponseSink& sink, std::string_view name, std::string_view value)
{
    if (equal_name(name, "x-request-id")) {
        const bool ascii = std::all_of(value.begin(), value.end(),
            [](unsigned char c) { return c >= 32 && c < 127; });
        if (ascii)
            sink.request_id.assign(value.substr(0, 95));
    } else if (equal_name(name, "retry-after")) {
        uint64_t seconds = 0;
        const auto [end, error] =
            std::from_chars(value.data(), value.data() + value.size(), seconds);
        if (error == std::errc{} && end == value.data() + value.size() &&
            seconds <= INT64_MAX / 1000)
            sink.retry_after_ms =
                std::max(sink.retry_after_ms, static_cast<int64_t>(seconds) * 1000);
    }
}

} // namespace

size_t collect_header(char* data, size_t size, size_t count, void* userdata) noexcept
{
    auto* sink = static_cast<ResponseSink*>(userdata);
    if (!sink || sink->failure != YM_OK)
        return 0;
    if (size && count > std::numeric_limits<size_t>::max() / size) {
        sink->failure = YM_RESOURCE_LIMIT;
        return 0;
    }
    const auto bytes = size * count;
    if (!bytes)
        return 0;
    if (!data) {
        sink->failure = YM_INTERNAL_ERROR;
        return 0;
    }
    if (sink->header_bytes > sink->max_headers || bytes > sink->max_headers - sink->header_bytes) {
        sink->failure = YM_RESOURCE_LIMIT;
        return 0;
    }
    sink->header_bytes += bytes;
    const std::string_view line(data, bytes);
    // Interim responses and proxy CONNECT blocks must not supply final metadata.
    if (line.starts_with("HTTP/")) {
        sink->request_id.clear();
        sink->retry_after_ms = -1;
        return bytes;
    }
    const auto colon = line.find(':');
    if (colon == std::string_view::npos)
        return bytes;
    const auto name = line.substr(0, colon);
    const auto value = trim(line.substr(colon + 1));
    try {
        read_metadata(*sink, name, value);
        return bytes;
    } catch (const std::bad_alloc&) {
        sink->failure = YM_OUT_OF_MEMORY;
    } catch (...) {
        sink->failure = YM_INTERNAL_ERROR;
    }
    return 0;
}

} // namespace ym::detail
