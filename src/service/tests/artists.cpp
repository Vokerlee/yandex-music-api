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
    CHECK(fixture.client.wait(fixture.client.artist("42")));
    CHECK(fixture.transport->input.url.ends_with("/artists/42/brief-info"));

    fixture.transport->body =
        R"({"result":{"tracks":[{"id":"123","title":"Song"}],"pager":{"total":125,"page":1,"perPage":50}}})";
    auto page = fixture.client.wait(fixture.client.artist_tracks("42", 1));
    CHECK(page && page->root().member("tracks")->size() == 1);
    CHECK(page->root().member("pager")->member("total")->unsigned_integer() == 125);
    CHECK(fixture.transport->input.url.ends_with("/artists/42/tracks?page=1&page-size=50"));

    CHECK(fixture.client.wait(fixture.client.artist_albums("42", 2, 20)));
    CHECK(fixture.transport->input.url.ends_with("/artists/42/direct-albums?page=2&page-size=20"));
    CHECK(fixture.client.wait(fixture.client.artist_similar("42")));
    CHECK(fixture.transport->input.url.ends_with("/artists/42/similar"));
    CHECK(!fixture.client.artist_tracks("42", 0, 0));
}
