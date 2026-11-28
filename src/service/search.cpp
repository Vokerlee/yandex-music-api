/*
 * Endpoint behavior references MarshalX/yandex-music-api at
 * 9ca132e5c18a72aab779c1fc68c169380a9357d0:
 * yandex_music/_client/search.py (search and suggestions).
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

#include <array>

#include <yandex_music/c/search.h>

ym_status YM_CALL ym_search(ym_client* client, ym_string_view query, ym_search_type type,
    uint32_t page, const ym_request_options* options, ym_request** out, ym_error* error) YM_NOEXCEPT
{
    return ym::detail::start_api(
        client,
        [&](auto& spec) {
            constexpr std::array types{"all", "track", "artist", "album", "playlist", "user",
                "podcast", "podcast_episode"};
            if (type >= types.size())
                throw std::invalid_argument("Unknown search type");
            spec.path = {"search"};
            spec.query = {
                ym::detail::field("text", ym::detail::text(query)),
                ym::detail::field("type", types[type]),
                ym::detail::field("page", std::to_string(page)),
                ym::detail::field("nocorrect", "False"),
                ym::detail::field("playlist-in-best", "True"),
            };
        },
        options, out, error
    );
}

ym_status YM_CALL ym_search_suggest(ym_client* client, ym_string_view query,
    const ym_request_options* options, ym_request** out, ym_error* error) YM_NOEXCEPT
{
    return ym::detail::start_api(
        client,
        [&](auto& spec) {
            spec.path = {"search", "suggest"};
            spec.query = {ym::detail::field("part", ym::detail::text(query))};
        },
        options, out, error
    );
}
