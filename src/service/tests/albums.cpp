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
    CHECK(fixture.client.wait(fixture.client.albums("8,9")));
    CHECK(fixture.transport->input.method == "POST");
    CHECK(fixture.transport->input.body == "album-ids=8%2C9");
    CHECK(!fixture.transport->input.mutating);

    fixture.transport->body =
        R"({"result":{"id":8,"title":"Album","volumes":[[{"id":"123"}],[{"id":"124"}]]}})";
    auto album = fixture.client.wait(fixture.client.album_tracks("8"));
    CHECK(album && album->root().member("title")->string() == "Album");
    CHECK(album->root().member("volumes")->size() == 2);
    CHECK(album->root().member("volumes")->at(1)->at(0)->member("id")->string() == "124");
    CHECK(fixture.transport->input.url.ends_with("/albums/8/with-tracks"));
}
