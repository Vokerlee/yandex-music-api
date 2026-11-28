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

#include <iostream>

#include <yandex_music/cpp/yandex_music.hpp>

int main()
{
    auto response = ym::parse_response(
        R"({"result":{"tracks":[{"id":42,"title":"Example"}],"available":true,"rating":4.5}})"
    );
    if (!response) {
        std::cerr << response.error().message() << '\n';
        return 1;
    }

    auto root = response->root();
    auto track = root["tracks"][size_t{0}];
    std::cout << track["id"].unsigned_integer().value_or(0) << '\t'
              << track["title"].string().value_or("") << '\n';
    std::cout << "Available: " << (root["available"].boolean().value_or(false) ? "yes" : "no")
              << '\n';
    std::cout << "Rating: " << root["rating"].number().value_or(0) << '\n';
    std::cout << "Optional field: " << (root["optional"] ? "present" : "missing") << '\n';
}
