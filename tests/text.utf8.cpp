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

#include "core/text.hpp"
#include "test_support.hpp"

int main()
{
    using ym::detail::valid_utf8;

    CHECK(valid_utf8("Бах") && valid_utf8("\xf4\x8f\xbf\xbf"));
    CHECK(!valid_utf8("\xc0\x80") && !valid_utf8("\xed\xa0\x80"));
    CHECK(!valid_utf8("\xf4\x90\x80\x80") && !valid_utf8("\xe2\x82"));
}
