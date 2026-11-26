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

#include "transport/curl_transfer.hpp"

#include <cstdlib>
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x))                                                                                  \
            std::abort();                                                                          \
    } while (false)
using namespace ym::detail;

int main()
{
    auto runtime = CurlRuntime::create();

    CHECK(runtime);

    RequestInput input{.method = "GET", .url = "https://api.music.yandex.net/account/status"};

    CHECK(prepare_transfer(*runtime, input, {}));

    for (const auto* url : {"http://api.music.yandex.net", "file:///tmp/test",
             "https://user@api.music.yandex.net", "https://api.music.yandex.net/#fragment"}) {
        input.url = url;
        auto invalid = prepare_transfer(*runtime, input, {});

        CHECK(!invalid && invalid.error().code == YM_INVALID_ARGUMENT);
    }
    CurlPolicy local;
    local.allow_loopback_http = true;
    input.url = "http://127.0.0.1:1/test";

    CHECK(prepare_transfer(*runtime, input, local));

    input.url = "http://example.com/test";

    CHECK(!prepare_transfer(*runtime, input, local));

    input.url = "https://api.music.yandex.net";
    local.max_body = 0;

    CHECK(!prepare_transfer(*runtime, input, local));
    CHECK(!prepare_transfer({}, input, {}));
}
