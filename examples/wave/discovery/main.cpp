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
    if (argc != 1 || !token || !*token) {
        std::cerr << "Usage: " << argv[0] << " \nSet YANDEX_MUSIC_TOKEN first.\n";
        return EXIT_FAILURE;
    }

    auto client = ym::Client::create({.token = token, .language = "en"});
    if (!client) {
        std::cerr << client.error().message() << '\n';
        return EXIT_FAILURE;
    }

    auto response = client->wait(client->wave_settings());
    if (!response) {
        std::cerr << response.error().message() << '\n';
        return EXIT_FAILURE;
    }

    auto root = response->root();
    auto choices = root["settingRestrictions"]["diversity"]["possibleValues"];
    std::string_view selected;
    const std::string_view wanted{"discover"};
    for (size_t i = 0; i < choices.size().value_or(0); ++i) {
        auto choice = choices[i];
        if (choice["value"].string().value_or("") == wanted ||
            choice["name"].string().value_or("") == wanted) {
            selected = choice["serializedSeed"].string().value_or("");
            break;
        }
    }
    if (selected.empty()) {
        std::cerr << "Choice unavailable; run the wave/settings example to list current options.\n";
        return EXIT_FAILURE;
    }
    auto wave = client->wait(client->wave_start_with("user:onyourwave", selected));
    if (!wave) {
        std::cerr << wave.error().message() << '\n';
        return EXIT_FAILURE;
    }
    auto wave_root = wave->root();
    print_value(wave_root["radioSessionId"]);
    std::cout << '\n';
    auto items = wave_root["sequence"];
    for (size_t i = 0; i < items.size().value_or(0); ++i) {
        auto record = items[i]["track"];
        print_value(record["id"]);
        std::cout << '\t';
        print_value(record["title"]);
        std::cout << '\n';
    }
    return EXIT_SUCCESS;
}
