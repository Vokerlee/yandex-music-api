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

#include <yandex_music/yandex_music.h>

int main()
{
    ym_client_options options;
    ym_client_options_init(&options);
    char language[] = "en";
    options.language = {language, 2};
    ym_client* client = nullptr;
    ym_error failure;

    CHECK(ym_client_create(&options, &client, &failure) == YM_OK);

    language[0] = 'x';
    auto copied = ym_client_language(client);

    CHECK(copied.size == 2 && std::memcmp(copied.data, "en", 2) == 0);

    ym_client_destroy(client);
    ym_client_destroy(nullptr);

    CHECK(ym_client_create(nullptr, nullptr, &failure) == YM_INVALID_ARGUMENT);

    options.token = {nullptr, 2};
    client = reinterpret_cast<ym_client*>(1);

    CHECK(ym_client_create(&options, &client, &failure) == YM_INVALID_ARGUMENT &&
        client == nullptr);
}
