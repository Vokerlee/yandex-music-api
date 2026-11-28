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

#include <yandex_music/c/request.h>

void YM_CALL ym_request_options_init(ym_request_options* options) YM_NOEXCEPT
{
    if (!options)
        return;
    *options = {};
    options->struct_size = sizeof(*options);
    options->version = 1;
    options->deadline_ms = UINT64_MAX;
}
