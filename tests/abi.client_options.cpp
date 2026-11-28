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
    ym_client_options options;
    ym_client_options_init(&options);

    CHECK(options.struct_size == sizeof(options) && options.version == 1);
    CHECK(options.token.size == 0 && options.token.data == nullptr);
    CHECK(options.language.size == 2 && options.max_response_bytes > 0 &&
        options.max_pending_requests > 0);

    ym_client_options_init(nullptr);
}
