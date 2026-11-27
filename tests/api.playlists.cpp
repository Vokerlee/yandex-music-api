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
        R"({"result":[{"kind":100,"title":"Favourites","owner":{"uid":123}}]})";
    auto list = fixture.client.wait(fixture.client.playlists("123"));
    CHECK(list && list->root().at(0)->member("title")->string() == "Favourites");
    CHECK(fixture.transport->input.url.ends_with("/users/123/playlists/list"));

    fixture.transport->body =
        R"({"result":{"kind":100,"revision":4,"tracks":[{"id":"42","albumId":"8","track":{"title":"Song"}}]}})";
    auto playlist = fixture.client.wait(fixture.client.playlist("123", "100"));
    CHECK(playlist && playlist->root().member("revision")->unsigned_integer() == 4);
    CHECK(playlist->root().member("tracks")->at(0)->member("track")->member("title")->string() ==
        "Song");
    CHECK(fixture.transport->input.url.ends_with("/users/123/playlists/100"));

    CHECK(fixture.client.wait(fixture.client.playlist_recommendations("123", "100")));
    CHECK(fixture.transport->input.url.ends_with("/users/123/playlists/100/recommendations"));
    CHECK(!fixture.client.playlist("", "100"));
}
