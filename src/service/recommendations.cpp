/*
 * Endpoint behavior references MarshalX/yandex-music-api at
 * 9ca132e5c18a72aab779c1fc68c169380a9357d0:
 * yandex_music/_client/landing.py and _client/playlists.py (personal recommendations).
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

#include <yandex_music/c/recommendations.h>

ym_status YM_CALL ym_recommendations(ym_client* client, const ym_request_options* options,
    ym_request** out, ym_error* error) YM_NOEXCEPT
{
    return ym::detail::start_api(
        client, [](auto& spec) { spec.path = {"feed"}; }, options, out, error
    );
}

ym_status YM_CALL ym_personal_playlist(ym_client* client, ym_string_view id,
    const ym_request_options* options, ym_request** out, ym_error* error) YM_NOEXCEPT
{
    return ym::detail::start_api(
        client,
        [&](auto& spec) { spec.path = {"playlists", "personal", ym::detail::identifier(id)}; },
        options, out, error
    );
}
