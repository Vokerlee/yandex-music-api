/*
 * Endpoint behavior references MarshalX/yandex-music-api at
 * 9ca132e5c18a72aab779c1fc68c169380a9357d0:
 * yandex_music/_client/wave.py and _client/radio.py (wave settings and station choices).
 * Copyright (C) 2019-2026 Ilya (Marshal) <https://github.com/MarshalX>
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

#include "core/api.hpp"

#include <yandex_music/c/wave.h>

ym_status YM_CALL ym_wave_settings(ym_client* client, ym_string_view seeds,
    const ym_request_options* options, ym_request** out, ym_error* error) YM_NOEXCEPT
{
    return ym::detail::start_api(
        client,
        [&](auto& spec) {
            spec.path = {"rotor", "wave", "settings"};
            if (seeds.size)
                spec.query = {ym::detail::field("seeds", ym::detail::text(seeds))};
        },
        options, out, error
    );
}

ym_status YM_CALL ym_wave_stations(ym_client* client, const ym_request_options* options,
    ym_request** out, ym_error* error) YM_NOEXCEPT
{
    return ym::detail::start_api(
        client,
        [&](auto& spec) {
            spec.path = {"rotor", "stations", "list"};
            spec.query = {ym::detail::field("language", client->language)};
        },
        options, out, error
    );
}
