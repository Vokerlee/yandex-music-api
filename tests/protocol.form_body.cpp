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
using namespace ym::detail;
using namespace ym::detail::wire;

int main()
{
    RequestSpec spec;
    spec.origin = "https://example.invalid";
    spec.path = {"batch"};
    spec.verb = HttpMethod::post;
    spec.encoding = BodyKind::form;
    spec.body = {
        {"track-ids", ym_value(std::string("9007199254740993:42,7"))},
        {"space",     ym_value(std::string("a +b"))                 }
    };

    CHECK(encode(spec)->body == "track-ids=9007199254740993%3A42%2C7&space=a+%2Bb");

    spec.body = {
        {"difference", ym_value(std::string("{\"op\":\"insert\"}"))}
    };

    CHECK(encode(spec)->body == "difference=%7B%22op%22%3A%22insert%22%7D");
    CHECK(encode(spec)->headers[0].second == "application/x-www-form-urlencoded");
}
