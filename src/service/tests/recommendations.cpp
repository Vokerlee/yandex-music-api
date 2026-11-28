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
        R"({"result":{"generatedPlaylists":[{"type":"daily","ready":true,"data":{"kind":100,"owner":{"uid":123}}}]}})";
    auto feed = fixture.client.wait(fixture.client.recommendations());
    CHECK(feed &&
        feed->root().member("generatedPlaylists")->at(0)->member("type")->string() == "daily");
    CHECK(fixture.transport->input.url == "https://api.music.yandex.net/feed");

    fixture.transport->body =
        R"({"result":{"type":"daily","data":{"tracks":[{"id":"42","track":{"title":"Recommended"}}]}}})";
    auto daily = fixture.client.wait(fixture.client.personal_playlist());
    CHECK(daily && daily->root().member("data")->member("tracks")->size() == 1);
    CHECK(fixture.transport->input.url.ends_with("/playlists/personal/daily"));
    CHECK(fixture.client.wait(fixture.client.personal_playlist("neverHeard")));
    CHECK(fixture.transport->input.url.ends_with("/playlists/personal/neverHeard"));
}
