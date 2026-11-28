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

#include "core/result.hpp"
#include "test_support.hpp"

#include <yandex_music/cpp/yandex_music.hpp>

int main()
{
    ym::Document owner(new ym_result{
        ym_value(ym_value::Object{{"items", ym_value(ym_value::Array{ym_value(true)})}}
        )
    });

    CHECK(*owner.root().size() == 1);
    CHECK(*owner.root().member("items")->at(0)->boolean());
    CHECK(owner.root().member("absent")->type() == YM_VALUE_MISSING);
    CHECK(!owner.root().at(0));
}
