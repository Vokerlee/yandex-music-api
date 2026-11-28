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

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <thread>

#include <yandex_music/cpp/yandex_music.hpp>

int main(int argc, char** argv)
{
    const char* token = std::getenv("YANDEX_MUSIC_TOKEN");
    if (argc != 2 || !token || !*token) {
        std::cerr << "Usage: " << argv[0] << " QUERY\nSet YANDEX_MUSIC_TOKEN first.\n";
        return 1;
    }
    auto client = ym::Client::create({.token = token, .language = "en"});
    if (!client) {
        std::cerr << client.error().message() << '\n';
        return 1;
    }
    ym_request_options options;
    ym_request_options_init(&options);
    options.deadline_ms = ym::monotonic_ms() + 30000;
    auto request = client->search(argv[1], ym::SearchType::track, 0, &options);
    if (!request) {
        std::cerr << request.error().message() << '\n';
        return 1;
    }
    while (request->status() == YM_PENDING) {
        auto advanced = client->poll();
        if (!advanced) {
            std::cerr << advanced.error().message() << '\n';
            return 1;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds{1});
    }
    auto response = request->take_result();
    if (!response) {
        std::cerr << response.error().message() << '\n';
        return 1;
    }
    std::cout << "HTTP " << response->http_status() << '\n';
}
