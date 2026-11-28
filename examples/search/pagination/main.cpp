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

#include <charconv>
#include <cstdlib>
#include <iostream>
#include <string_view>

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
    if (argc != 3 || !token || !*token) {
        std::cerr << "Usage: " << argv[0] << " QUERY PAGE\nSet YANDEX_MUSIC_TOKEN first.\n";
        return EXIT_FAILURE;
    }

    uint32_t page = 0;
    std::string_view text = argv[2];
    auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), page);
    if (error != std::errc{} || end != text.data() + text.size()) {
        std::cerr << "PAGE must be an unsigned 32-bit integer.\n";
        return EXIT_FAILURE;
    }
    auto client = ym::Client::create({.token = token, .language = "en"});
    if (!client) {
        std::cerr << client.error().message() << '\n';
        return EXIT_FAILURE;
    }

    auto response = client->wait(client->search(argv[1], ym::SearchType::track, page));
    if (!response) {
        std::cerr << response.error().message() << '\n';
        return EXIT_FAILURE;
    }

    auto root = response->root();
    auto items = root["tracks"]["results"];
    for (size_t i = 0; i < items.size().value_or(0); ++i) {
        auto record = items[i];
        print_value(record["id"]);
        std::cout << '\t';
        print_value(record["title"]);
        std::cout << '\n';
    }
    return EXIT_SUCCESS;
}
