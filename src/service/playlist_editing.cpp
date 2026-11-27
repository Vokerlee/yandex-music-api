/*
 * Endpoint behavior references MarshalX/yandex-music-api at
 * 9ca132e5c18a72aab779c1fc68c169380a9357d0:
 * yandex_music/_client/playlists.py (playlist creation and deletion).
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

#include <yandex_music/playlists.h>

ym_status YM_CALL ym_playlist_create(ym_client* client, ym_string_view user, ym_string_view title,
    ym_string_view visibility, const ym_request_options* options, ym_request** out,
    ym_error* error) YM_NOEXCEPT
{
    return ym::detail::start_api(
        client,
        [&](auto& spec) {
            auto access = ym::detail::text(visibility);
            if (access != "public" && access != "private")
                throw std::invalid_argument("Visibility must be public or private");
            spec.path = {"users", ym::detail::identifier(user), "playlists", "create"};
            spec.verb = ym::detail::wire::HttpMethod::post;
            spec.mutating = true;
            spec.encoding = ym::detail::wire::BodyKind::form;
            spec.body = {ym::detail::field("title", ym::detail::text(title)),
                ym::detail::field("visibility", std::move(access))};
        },
        options, out, error
    );
}

ym_status YM_CALL ym_playlist_delete(ym_client* client, ym_string_view user, ym_string_view kind,
    const ym_request_options* options, ym_request** out, ym_error* error) YM_NOEXCEPT
{
    return ym::detail::start_api(
        client,
        [&](auto& spec) {
            spec.path = {"users", ym::detail::identifier(user), "playlists",
                ym::detail::identifier(kind), "delete"};
            spec.verb = ym::detail::wire::HttpMethod::post;
            spec.mutating = true;
        },
        options, out, error
    );
}
