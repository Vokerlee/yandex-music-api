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
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x))                                                                                  \
            std::abort();                                                                          \
    } while (false)
using namespace ym::detail;

int main()
{
    ResponseSink sink(4);
    char bytes[] = {'a', 0, 'b', 'c', 'd'};

    CHECK(collect_body(bytes, 1, 2, &sink) == 2);
    CHECK(collect_body(bytes + 2, 1, 2, &sink) == 2);
    CHECK(sink.body == std::string(bytes, 4));
    CHECK(collect_body(bytes, 1, 1, &sink) == 0 && sink.failure == YM_RESOURCE_LIMIT);
    CHECK(sink.body.size() == 4);

    ResponseSink overflow(4);

    CHECK(collect_body(bytes, 2, std::numeric_limits<size_t>::max(), &overflow) == 0);
    CHECK(overflow.failure == YM_RESOURCE_LIMIT && overflow.body.empty());

    ResponseSink empty(0);

    CHECK(collect_body(nullptr, 0, 9, &empty) == 0 && empty.failure == YM_OK);
    CHECK(collect_body(bytes, 1, 1, &empty) == 0 && empty.failure == YM_RESOURCE_LIMIT);
}
