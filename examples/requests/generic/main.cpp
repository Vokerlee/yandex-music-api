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
    if (argc != 1 || !token || !*token) {
        std::cerr << "Usage: " << argv[0] << " \nSet YANDEX_MUSIC_TOKEN first.\n";
        return EXIT_FAILURE;
    }

    const ym_string_view path[]{
        {"account", 7},
        {"status",  6}
    };
    ym_api_call call{};
    call.method = {"GET", 3};
    call.path = path;
    call.path_count = 2;
    auto client = ym::Client::create({.token = token, .language = "en"});
    if (!client) {
        std::cerr << client.error().message() << '\n';
        return EXIT_FAILURE;
    }

    auto response = client->wait(client->request(call));
    if (!response) {
        std::cerr << response.error().message() << '\n';
        return EXIT_FAILURE;
    }

    auto root = response->root();
    std::cout << root["account"]["login"].string().value_or("") << '\n';
    std::cout << "HTTP " << response->http_status() << '\n' << response->request_id() << '\n';
    return EXIT_SUCCESS;
}
