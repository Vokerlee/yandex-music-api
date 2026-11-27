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

#include "transport/response_sink.hpp"

#include <cstdlib>
#include <limits>
#include <string_view>
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x))                                                                                  \
            std::abort();                                                                          \
    } while (false)
using namespace ym::detail;

int main()
{
    ResponseSink sink(4);
    auto add = [&](std::string_view line) {
        return collect_header(const_cast<char*>(line.data()), 1, line.size(), &sink);
    };

    CHECK(add("HTTP/1.1 100 Continue\r\n"));
    CHECK(add("X-ReQuEsT-ID: first\r\n") && sink.request_id == "first");
    CHECK(add("Retry-After: 2\r\n") && sink.retry_after_ms == 2000);
    CHECK(add("Retry-After: 1\r\n") && sink.retry_after_ms == 2000);
    CHECK(add("HTTP/2 429\r\n") && sink.request_id.empty() && sink.retry_after_ms == -1);
    CHECK(add("Retry-After: 18446744073709551615\r\n") && sink.retry_after_ms == -1);
    CHECK(add("Retry-After: -1\r\n") && sink.retry_after_ms == -1);
    CHECK(add("Retry-After: tomorrow\r\n") && sink.retry_after_ms == -1);
    CHECK(add("X-Request-Id: final\r\n") && sink.request_id == "final");

    sink.max_headers = sink.header_bytes + 2;

    CHECK(add("\r\n") == 2 && sink.failure == YM_OK);
    CHECK(add("a") == 0 && sink.failure == YM_RESOURCE_LIMIT);

    ResponseSink overflow(4);
    char byte = 'x';

    CHECK(collect_header(&byte, 2, std::numeric_limits<size_t>::max(), &overflow) == 0);
    CHECK(overflow.failure == YM_RESOURCE_LIMIT);
}
