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

#include <cstdlib>
#include <iostream>

#include <yandex_music/cpp/yandex_music.hpp>

int main(int argc, char** argv)
{
    const char* token = std::getenv("YANDEX_MUSIC_TOKEN");
    if (argc != 3 || !token || !*token) {
        std::cerr << "Usage: " << argv[0] << " TRACK_ID FILENAME\nSet YANDEX_MUSIC_TOKEN first.\n";
        return 1;
    }
    auto client = ym::Client::create({.token = token, .language = "en"});
    if (!client) {
        std::cerr << client.error().message() << '\n';
        return 1;
    }
    auto saved = client->download_track(argv[1], argv[2]);
    if (!saved) {
        std::cerr << saved.error().message() << '\n';
        return 1;
    }
    std::cout << "Saved " << argv[2] << '\n';
}
