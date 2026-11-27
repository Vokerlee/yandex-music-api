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

#include "protocol/encode.hpp"
#include "test_support.hpp"
using namespace ym::detail::wire;

int main()
{
    RequestSpec spec;
    spec.origin = "https://example.invalid";
    spec.headers = {
        {"X-Test", "value"}
    };

    CHECK(encode(spec)->headers[0].second == "value");

    spec.headers = {
        {"X-Test", "one"},
        {"x-test", "two"}
    };

    CHECK(!encode(spec));

    spec.headers = {
        {"X-Test", "value\r\nInjected: true"}
    };

    CHECK(!encode(spec));

    spec.headers = {
        {"Bad Name", "value"}
    };

    CHECK(!encode(spec));

    spec.headers.clear();

    CHECK(!encode(spec, 10) && encode(spec, 10).error().code == YM_RESOURCE_LIMIT);
}
