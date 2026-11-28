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
    fixture.transport->body =
        R"({"result":[{"id":"123","title":"Song","durationMs":123456,"artists":[{"id":7,"name":"Artist"}],"albums":[{"id":8}],"coverUri":"example/%%"}]})";
    auto tracks = fixture.client.wait(fixture.client.tracks("123:8,456"));
    CHECK(tracks && tracks->root().size() == 1);
    CHECK(fixture.transport->input.method == "POST");
    CHECK(std::ranges::find(fixture.transport->input.headers,
              std::pair<std::string, std::string>{"X-Yandex-Music-Client",
                  "YandexMusicAndroid/24023621"}) != fixture.transport->input.headers.end());
    CHECK(std::ranges::find(fixture.transport->input.headers,
              std::pair<std::string, std::string>{"User-Agent", "Yandex-Music-Native"}) !=
        fixture.transport->input.headers.end());
    CHECK(fixture.transport->input.body == "track-ids=123%3A8%2C456&with-positions=True");
    auto first = tracks->root().at(0);
    CHECK(first && first->member("title")->string() == "Song");
    CHECK(first->member("durationMs")->unsigned_integer() == 123456);
    CHECK(first->member("artists")->at(0)->member("name")->string() == "Artist");

    CHECK(fixture.client.wait(fixture.client.track_similar("123")));
    CHECK(fixture.transport->input.url == "https://api.music.yandex.net/tracks/123/similar");
    CHECK(fixture.client.wait(fixture.client.track_supplement("123")));
    CHECK(fixture.transport->input.url.ends_with("/tracks/123/supplement"));
    CHECK(!fixture.client.tracks(""));
}
