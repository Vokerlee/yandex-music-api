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
    spec.path = {"users", "owner/name", "playlists", "kind:one"};
    spec.query = {
        {"text", ym_value(std::string("Бах +&?"))}
    };
    auto encoded = encode(spec);

    CHECK(encoded);
    CHECK(encoded->url ==
        "https://example.invalid/users/owner%2Fname/playlists/"
        "kind%3Aone?text=%D0%91%D0%B0%D1%85%20%2B%26%3F");
    CHECK(encoded->method == "GET" && !encoded->mutating);

    spec.verb = HttpMethod::post;

    CHECK(!encode(spec)->mutating);

    spec.origin = "https://user@example.invalid";

    CHECK(!encode(spec));

    spec.origin = "https://example.invalid/path";

    CHECK(!encode(spec));
}
