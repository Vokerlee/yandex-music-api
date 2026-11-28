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

#include <yandex_music/c/yandex_music.h>

int main()
{
    struct Prefix {
        size_t size;
        uint32_t version;
    } old{sizeof(old), 1};

    ym_client* client = nullptr;
    ym_error error;

    CHECK(ym_client_create(reinterpret_cast<const ym_client_options*>(&old), &client, &error) ==
        YM_OK);

    ym_client_destroy(client);

    struct Extended {
        ym_client_options known;
        uint64_t tail;
    } future{};

    ym_client_options_init(&future.known);
    future.known.struct_size = sizeof(future);

    CHECK(ym_client_create(&future.known, &client, &error) == YM_OK);

    ym_client_destroy(client);
    future.known.version = 99;

    CHECK(ym_client_create(&future.known, &client, &error) == YM_UNSUPPORTED && client == nullptr);

    future.known.struct_size = 1;

    CHECK(ym_client_create(&future.known, &client, &error) == YM_INVALID_ARGUMENT);
}
