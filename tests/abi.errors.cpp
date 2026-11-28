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

#include <yandex_music/c/yandex_music.h>

int main()
{
    ym_error value;
    ym_error_init(&value);

    CHECK(value.code == YM_OK && value.retry_after_ms == -1);
    CHECK(value.message[0] == 0 && value.server_code[0] == 0);

    value.code = YM_UNAUTHORIZED;
    std::strcpy(value.message, "owned detail");
    const auto saved = value;
    ym_error_init(&value);

    CHECK(saved.code == YM_UNAUTHORIZED && std::strcmp(saved.message, "owned detail") == 0);

    CHECK(std::strcmp(ym_status_name(YM_CANCELLED), "YM_CANCELLED") == 0);
    CHECK(std::strcmp(ym_status_name(999), "YM_UNKNOWN_STATUS") == 0);

    ym_error_init(nullptr);
}
