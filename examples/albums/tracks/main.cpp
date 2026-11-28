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

void print_value(ym::ValueView value)
{
    if (auto text = value.string())
        std::cout << *text;
    else if (auto number = value.unsigned_integer())
        std::cout << *number;
}

int main(int argc, char** argv)
{
    const char* token = std::getenv("YANDEX_MUSIC_TOKEN");
    if (argc != 2 || !token || !*token) {
        std::cerr << "Usage: " << argv[0] << " ALBUM_ID\nSet YANDEX_MUSIC_TOKEN first.\n";
        return EXIT_FAILURE;
    }

    auto client = ym::Client::create({.token = token, .language = "en"});
    if (!client) {
        std::cerr << client.error().message() << '\n';
        return EXIT_FAILURE;
    }

    auto response = client->wait(client->album_tracks(argv[1]));
    if (!response) {
        std::cerr << response.error().message() << '\n';
        return EXIT_FAILURE;
    }

    auto root = response->root();
    auto volumes = root["volumes"];
    for (size_t disc = 0; disc < volumes.size().value_or(0); ++disc) {
        std::cout << "Disc " << disc + 1 << '\n';
        auto tracks = volumes[disc];
        for (size_t i = 0; i < tracks.size().value_or(0); ++i) {
            print_value(tracks[i]["id"]);
            std::cout << '\t';
            print_value(tracks[i]["title"]);
            std::cout << '\n';
        }
    }
    return EXIT_SUCCESS;
}
