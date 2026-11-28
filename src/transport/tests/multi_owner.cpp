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

#include "transport/curl_multi.hpp"

#include <cstdlib>
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x))                                                                                  \
            std::abort();                                                                          \
    } while (false)
using namespace ym::detail;

int main()
{
    CurlPolicy policy;
    policy.allow_loopback_http = true;
    auto owner = CurlMultiOwner::create(policy);

    CHECK(owner);

    RequestInput input{.method = "POST", .url = "http://127.0.0.1:1/test", .body = "borrowed"};

    CHECK(!(*owner)->start(0, input));
    CHECK((*owner)->start(1, input));
    CHECK((*owner)->active.size() == 1);
    CHECK(!(*owner)->start(1, input));
    CHECK((*owner)->active.size() == 1);

    (*owner)->cancel(1);
    (*owner)->cancel(1);

    CHECK((*owner)->active.empty());

    input.body.clear();
    input.body.shrink_to_fit();

    CHECK((*owner)->start(2, input));

    // Destruction also detaches a still-active transfer without performing I/O.
    owner->reset();
    policy.max_body = 0;

    CHECK(!CurlMultiOwner::create(policy));
}
