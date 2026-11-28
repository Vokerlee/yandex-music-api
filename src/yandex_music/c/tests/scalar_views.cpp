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

#include <yandex_music/c/yandex_music.h>

int main()
{
    ym_value exact{uint64_t{9007199254740993ULL}};
    uint64_t integer = 0;

    CHECK(ym_value_uint64(&exact, &integer, nullptr) == YM_OK && integer == 9007199254740993ULL);

    double number = 9;

    CHECK(ym_value_double(&exact, &number, nullptr) == YM_SCHEMA_ERROR && number == 0);

    ym_value null;

    CHECK(ym_value_typeof(&null) == YM_VALUE_NULL && ym_value_typeof(nullptr) == YM_VALUE_MISSING);

    ym_value empty{std::string{}};
    ym_string_view view{};

    CHECK(ym_value_string(&empty, &view, nullptr) == YM_OK && view.size == 0);

    ym_value flag{false};
    uint32_t boolean = 1;

    CHECK(ym_value_bool(&flag, &boolean, nullptr) == YM_OK && boolean == 0);

    ym_value zero{int64_t{0}};
    int64_t signed_value = 1;

    CHECK(ym_value_int64(&zero, &signed_value, nullptr) == YM_OK && signed_value == 0);
}
