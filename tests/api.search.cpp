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

int main()
{
    ApiFixture fixture;
    fixture.transport->body =
        R"({"result":{"tracks":{"total":100,"perPage":20,"results":[{"id":"1","title":"Found"}]}}})";
    auto result = fixture.client.wait(fixture.client.search("an artist", ym::SearchType::track, 2));
    CHECK(result);
    CHECK(fixture.transport->input.url ==
        "https://api.music.yandex.net/"
        "search?text=an%20artist&type=track&page=2&nocorrect=False&playlist-in-best=True");
    auto tracks = result->root().member("tracks");
    CHECK(tracks && tracks->member("total")->unsigned_integer() == 100);
    CHECK(tracks->member("results")->at(0)->member("title")->string() == "Found");

    CHECK(fixture.client.wait(fixture.client.search("name", ym::SearchType::artist)));
    CHECK(fixture.transport->input.url.find("type=artist") != std::string::npos);
    CHECK(fixture.client.wait(fixture.client.search_suggest("Pink Floyd")));
    CHECK(fixture.transport->input.url ==
        "https://api.music.yandex.net/search/suggest?part=Pink%20Floyd");
    CHECK(result->root()["tracks"]["results"][0]["title"].string() == "Found");
    CHECK(!result->root()["missing"]["results"]);
    CHECK(!fixture.client.search("name", static_cast<ym::SearchType>(99)));
}
