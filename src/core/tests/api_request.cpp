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

#include "api_support.hpp"

#include <algorithm>

int main()
{
    ApiFixture fixture;
    std::string term{"hello & мир"};
    const ym_string_view path[]{
        {"search", 6}
    };
    const ym_api_field query[]{
        {{"text", 4}, {term.data(), term.size()}}
    };
    ym_api_call call{};
    call.method = {"GET", 3};
    call.path = path;
    call.path_count = 1;
    call.query = query;
    call.query_count = 1;
    auto request = fixture.client.request(call);
    term.assign("changed");
    auto document = fixture.complete(std::move(request));
    const auto& input = fixture.transport->input;
    CHECK(input.url == "https://api.music.yandex.net/search?text=hello%20%26%20%D0%BC%D0%B8%D1%80");
    CHECK(std::ranges::find(input.headers,
              std::pair<std::string, std::string>{"Authorization", "OAuth test-token"}) !=
        input.headers.end());
    CHECK(document && document->root().member("ok")->boolean() == true);

    call.method = {"POST", 4};
    call.query_count = 0;
    call.json = {R"({"values":[1,true]})", 19};
    CHECK(fixture.complete(fixture.client.request(call)));
    CHECK(fixture.transport->input.body == R"({"values":[1,true]})");
    CHECK(fixture.transport->input.mutating);

    const ym_string_view invalid[]{
        {"..", 2}
    };
    call.path = invalid;
    CHECK(!fixture.client.request(call));
    call.path = path;
    call.json = {"{", 1};
    auto invalid_json = fixture.client.request(call);
    CHECK(!invalid_json && invalid_json.error().code() == YM_SCHEMA_ERROR);
    call.json = {};
    fixture.transport->http_status = 401;
    auto denied = fixture.client.request(call);
    CHECK(denied && fixture.client.poll(++fixture.now));
    CHECK(denied->take_result().error().code() == YM_UNAUTHORIZED);
}
