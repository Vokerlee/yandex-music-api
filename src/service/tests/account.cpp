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
        R"({"result":{"account":{"uid":123,"login":"listener"},"plus":{"hasPlus":true}}})";
    auto status = fixture.client.wait(fixture.client.account_status());
    CHECK(status && status->root().member("account")->member("uid")->unsigned_integer() == 123);
    CHECK(fixture.transport->input.url.ends_with("/account/status"));

    fixture.transport->body =
        R"({"result":{"library":{"revision":7,"tracks":[{"id":"42","albumId":"8"}]}}})";
    auto tracks = fixture.client.wait(fixture.client.likes("123"));
    CHECK(tracks && tracks->root().member("library")->member("tracks")->size() == 1);
    CHECK(fixture.transport->input.url.ends_with(
        "/users/123/likes/tracks?if-modified-since-revision=0"
    ));
    CHECK(fixture.client.wait(fixture.client.likes("123", ym::LibraryType::artist)));
    CHECK(fixture.transport->input.url.ends_with("/users/123/likes/artists?with-timestamps=True"));
    CHECK(fixture.client.wait(fixture.client.likes("123", ym::LibraryType::album)));
    CHECK(fixture.transport->input.url.ends_with("/users/123/likes/albums?rich=True"));

    CHECK(fixture.client.wait(fixture.client.set_like("123", ym::LibraryType::track, "42,43")));
    CHECK(fixture.transport->input.url.ends_with("/users/123/likes/tracks/add-multiple"));
    CHECK(fixture.transport->input.body == "track-ids=42%2C43");
    CHECK(fixture.transport->input.mutating);
    CHECK(fixture.client.wait(fixture.client.set_like("123", ym::LibraryType::playlist, "123:100",
        false)));
    CHECK(fixture.transport->input.url.ends_with("/users/123/likes/playlists/remove"));
    CHECK(fixture.transport->input.body == "playlist-ids=123%3A100");
}
