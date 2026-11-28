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

#include "protocol/percent.hpp"
#include "test_support.hpp"
using namespace ym::detail::wire;

int main()
{
    CHECK(*percent_encode("Бах +/&?", false, 128) == "%D0%91%D0%B0%D1%85%20%2B%2F%26%3F");
    CHECK(*percent_encode("a +b", true, 128) == "a+%2Bb");
    CHECK(*percent_encode("-_.~", false, 128) == "-_.~");

    CHECK(!percent_encode(std::string("a\0b", 3), false, 128));
    CHECK(percent_encode("Бах", false, 2).error().code == YM_RESOURCE_LIMIT);
}
