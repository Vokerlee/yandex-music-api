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

#include <cstring>

#include <yandex_music/yandex_music.hpp>

int main()
{
    ym_error raw;
    ym_error_init(&raw);
    raw.code = YM_NOT_FOUND;
    std::strcpy(raw.message, "missing");

    ym::result<int> result = std::unexpected(ym::error(raw));
    ym_error_init(&raw);

    CHECK(
        !result && result.error().code() == YM_NOT_FOUND && result.error().message() == "missing");

    result = 42;

    CHECK(*result == 42);
}
