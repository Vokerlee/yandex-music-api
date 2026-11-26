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
    ym_client* client = nullptr;

    CHECK(ym_client_create(nullptr, &client, nullptr) == YM_OK);

    auto* original = new ym_result{ym_value{std::string{"owned"}}};
    ym_client_destroy(client);
    ym_result* copy = nullptr;

    CHECK(ym_result_clone(original, &copy, nullptr) == YM_OK);

    ym_result_destroy(original);

    CHECK(std::get<std::string>(ym_result_root(copy)->data) == "owned");
    CHECK(ym_value_typeof(ym_result_root(copy)) == YM_VALUE_STRING);

    ym_result_destroy(copy);
    ym_result_destroy(nullptr);
    copy = reinterpret_cast<ym_result*>(1);

    CHECK(ym_result_clone(nullptr, &copy, nullptr) == YM_INVALID_ARGUMENT && copy == nullptr);
}
