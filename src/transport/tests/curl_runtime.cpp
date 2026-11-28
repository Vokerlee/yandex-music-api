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

#include "transport/curl_runtime.hpp"

#include <cstdlib>
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x))                                                                                  \
            std::abort();                                                                          \
    } while (false)

int main()
{
    // Independent leases can be released in any order and re-acquired.
    auto first = ym::detail::CurlRuntime::create();

    CHECK(first);

    auto second = ym::detail::CurlRuntime::create();

    CHECK(second);

    first->reset();

    CHECK(*second);

    auto third = ym::detail::CurlRuntime::create();

    CHECK(third);

    second->reset();
    third->reset();

    CHECK(ym::detail::CurlRuntime::create());
}
