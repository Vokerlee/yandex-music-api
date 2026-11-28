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
        R"({"result":{"kind":100,"title":"Road & rain","visibility":"private"}})";
    auto created = fixture.client.wait(fixture.client.playlist_create("123", "Road & rain"));
    CHECK(created && created->root().member("kind")->unsigned_integer() == 100);
    CHECK(fixture.transport->input.url.ends_with("/users/123/playlists/create"));
    CHECK(fixture.transport->input.body == "title=Road+%26+rain&visibility=private");
    CHECK(fixture.transport->input.mutating);

    fixture.transport->body = R"({"result":"ok"})";
    auto deleted = fixture.client.wait(fixture.client.playlist_delete("123", "100"));
    CHECK(deleted && deleted->root().string() == "ok");
    CHECK(fixture.transport->input.method == "POST");
    CHECK(fixture.transport->input.url.ends_with("/users/123/playlists/100/delete"));
    CHECK(fixture.transport->input.mutating);
    CHECK(!fixture.client.playlist_create("123", "Title", "unknown"));
}
