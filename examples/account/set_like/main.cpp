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
#include <string_view>

#include <yandex_music/cpp/yandex_music.hpp>

int main(int argc, char** argv)
{
    const char* token = std::getenv("YANDEX_MUSIC_TOKEN");
    if (argc != 4 || !token || !*token) {
        std::cerr << "Usage: " << argv[0]
                  << " USER_ID TRACK_IDS like_or_unlike\nSet YANDEX_MUSIC_TOKEN first.\n";
        return EXIT_FAILURE;
    }

    const std::string_view action{argv[3]};
    if (action != "like" && action != "unlike") {
        std::cerr << "Choose like or unlike\n";
        return EXIT_FAILURE;
    }
    const bool liked = action == "like";
    auto client = ym::Client::create({.token = token, .language = "en"});
    if (!client) {
        std::cerr << client.error().message() << '\n';
        return EXIT_FAILURE;
    }

    auto response = client->wait(client->set_like(argv[1], ym::LibraryType::track, argv[2], liked));
    if (!response) {
        std::cerr << response.error().message() << '\n';
        return EXIT_FAILURE;
    }

    std::cout << "Updated\n";
    return EXIT_SUCCESS;
}
