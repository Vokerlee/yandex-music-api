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

#include <yandex_music/yandex_music.h>

int main()
{
    ym_value object{ym_value::Object{{"items", ym_value{ym_value::Array{ym_value{false}}}}}};
    const ym_value* items = nullptr;
    const ym_value* item = nullptr;
    size_t size = 0;

    CHECK(ym_value_size(&object, &size, nullptr) == YM_OK && size == 1);
    CHECK(ym_value_member(&object, {"items", 5}, &items, nullptr) == YM_OK);
    CHECK(ym_value_size(items, &size, nullptr) == YM_OK && size == 1);
    CHECK(ym_value_at(items, 0, &item, nullptr) == YM_OK && ym_value_typeof(item) == YM_VALUE_BOOL);
    CHECK(ym_value_at(items, 1, &item, nullptr) == YM_NOT_FOUND && item == nullptr);
    CHECK(ym_value_member(&object, {"missing", 7}, &item, nullptr) == YM_NOT_FOUND &&
        item == nullptr);
    CHECK(ym_value_at(&object, 0, &item, nullptr) == YM_SCHEMA_ERROR);
}
