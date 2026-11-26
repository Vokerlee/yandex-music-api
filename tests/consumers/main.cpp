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

#include <yandex_music/yandex_music.hpp>
#include <iostream>

int main()
{
    if (ym::version() != "0.1.0" || ym::build_info().empty()) {
        std::cerr << "Invalid C++ version/build contract\n";
        return 1;
    }
}
