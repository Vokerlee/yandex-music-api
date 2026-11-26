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

#include "test_support.hpp"

#include <type_traits>
#include <utility>

#include <yandex_music/yandex_music.hpp>

int main()
{
    static_assert(!std::is_copy_constructible_v<ym::Client>);
    static_assert(std::is_nothrow_move_constructible_v<ym::Client>);
    static_assert(std::is_nothrow_move_assignable_v<ym::Client>);

    char language[] = "en";
    auto owner = ym::Client::create({.language = language});

    CHECK(owner);

    language[0] = 'x';

    CHECK(owner->language() == "en");

    auto moved = std::move(*owner);

    CHECK(owner->language().empty() && moved.language() == "en");
}
