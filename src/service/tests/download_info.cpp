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
#include "service/file_signature.hpp"

int main()
{
    CHECK(ym::detail::file_signature("1700000000", "123:8", "hq", "mp3") ==
        "nzEPVUvcDaaf2nchAqVbC/itwZe9C2BhY9s6/lw1OGM");
    ApiFixture fixture;
    CHECK(fixture.client.wait(fixture.client.track_download_info("123")));
    CHECK(fixture.transport->input.url.ends_with("/tracks/123/download-info"));

    fixture.transport->body =
        R"({"result":{"downloadInfo":{"trackId":"123:8","quality":"hq","codec":"mp3","transport":"raw","url":"https://storage.example/track"}}})";
    auto info = fixture.client.wait(fixture.client.track_file_info("123:8"));
    CHECK(info &&
        info->root().member("downloadInfo")->member("url")->string() ==
            "https://storage.example/track");
    CHECK(
        fixture.transport->input.url.starts_with("https://api.music.yandex.net/get-file-info?ts=")
    );
    CHECK(fixture.transport->input.url.find(
              "trackId=123%3A8&quality=hq&codecs=mp3&transports=raw&sign="
          ) != std::string::npos);
    bool desktop_header = false;
    for (const auto& [key, value] : fixture.transport->input.headers)
        if (key == "X-Yandex-Music-Client" && value == "YandexMusicDesktopAppWindows/5.95.0")
            desktop_header = true;
    CHECK(desktop_header);
}
