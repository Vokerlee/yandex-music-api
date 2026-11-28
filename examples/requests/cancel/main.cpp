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
    auto client = ym::Client::create();
    if (!client) {
        std::cerr << client.error().message() << '\n';
        return 1;
    }
    auto request = client->search("music", ym::SearchType::track);
    if (!request) {
        std::cerr << request.error().message() << '\n';
        return 1;
    }
    auto cancelled = request->cancel();
    if (!cancelled) {
        std::cerr << cancelled.error().message() << '\n';
        return 1;
    }
    std::cout << ym_status_name(request->status()) << '\n';
}
