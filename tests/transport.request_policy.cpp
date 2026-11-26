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

    RequestInput input{.method = "GET", .url = "https://api.music.yandex.net"};
    auto rejected = [&] {
        auto r = prepare_transfer(*runtime, input, {});
        return !r && r.error().code == YM_INVALID_ARGUMENT;
    };
    input.method = "TRACE";

    CHECK(rejected());

    input.method = "GET";
    input.body = "unexpected";

    CHECK(rejected());

    input.method = "HEAD";

    CHECK(rejected());

    input.method = "POST";
    input.body.clear();
    for (const auto* name : {"bad name", "x:injected", "x\r\ny:evil", ""}) {
        input.headers = {
            {name, "value"}
        };

        CHECK(rejected());
    }
    input.headers = {
        {"X-Safe", std::string("a\0b", 3)}
    };

    CHECK(rejected());

    input.headers = {
        {"X-Safe", "a\r\nb"}
    };

    CHECK(rejected());

    input.headers = {
        {"X-Empty", ""}
    };
    input.body = std::string("a\0b", 3);

    CHECK(prepare_transfer(*runtime, input, {}));
}
