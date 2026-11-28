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

void YM_CALL complete(ym_request*, ym_status status, void* userdata)
{
    *static_cast<int*>(userdata) = 1;
    std::cout << "Completed: " << ym_status_name(status) << '\n';
}

int main(int argc, char** argv)
{
    const char* token = std::getenv("YANDEX_MUSIC_TOKEN");
    if (argc != 2 || !token || !*token) {
        std::cerr << "Usage: " << argv[0] << " QUERY\nSet YANDEX_MUSIC_TOKEN first.\n";
        return EXIT_FAILURE;
    }

    int done = 0;
    ym_request_options request_options;
    ym_request_options_init(&request_options);
    request_options.on_complete = complete;
    request_options.userdata = &done;
    auto client = ym::Client::create({.token = token, .language = "en"});
    if (!client) {
        std::cerr << client.error().message() << '\n';
        return EXIT_FAILURE;
    }

    auto response =
        client->wait(client->search(argv[1], ym::SearchType::track, 0, &request_options));
    if (!response) {
        std::cerr << response.error().message() << '\n';
        return EXIT_FAILURE;
    }

    auto root = response->root();
    std::cout << "HTTP " << response->http_status() << '\n';
    std::cout << "Callback delivered: " << (done ? "yes" : "no") << '\n';
    (void)root;
    return EXIT_SUCCESS;
}
