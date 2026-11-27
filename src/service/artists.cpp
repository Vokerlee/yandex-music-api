/*
 * Endpoint behavior references MarshalX/yandex-music-api at
 * 9ca132e5c18a72aab779c1fc68c169380a9357d0:
 * yandex_music/_client/artists.py (artist browsing and pagination).
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

#include <yandex_music/catalogue.h>

namespace {

ym_status artist_page(ym_client* client, ym_string_view id, const char* collection, uint32_t page,
    uint32_t page_size, const ym_request_options* options, ym_request** out,
    ym_error* error) noexcept
{
    return ym::detail::start_api(
        client,
        [&](auto& spec) {
            if (!page_size)
                throw std::invalid_argument("Page size must be positive");
            spec.path = {"artists", ym::detail::identifier(id), collection};
            spec.query = {ym::detail::field("page", std::to_string(page)),
                ym::detail::field("page-size", std::to_string(page_size))};
        },
        options, out, error
    );
}

} // namespace

ym_status YM_CALL ym_artist(ym_client* client, ym_string_view id, const ym_request_options* options,
    ym_request** out, ym_error* error) YM_NOEXCEPT
{
    return ym::detail::start_api(
        client,
        [&](auto& spec) { spec.path = {"artists", ym::detail::identifier(id), "brief-info"}; },
        options, out, error
    );
}

ym_status YM_CALL ym_artist_tracks(ym_client* client, ym_string_view id, uint32_t page,
    uint32_t page_size, const ym_request_options* options, ym_request** out,
    ym_error* error) YM_NOEXCEPT
{
    return artist_page(client, id, "tracks", page, page_size, options, out, error);
}

ym_status YM_CALL ym_artist_albums(ym_client* client, ym_string_view id, uint32_t page,
    uint32_t page_size, const ym_request_options* options, ym_request** out,
    ym_error* error) YM_NOEXCEPT
{
    return artist_page(client, id, "direct-albums", page, page_size, options, out, error);
}

ym_status YM_CALL ym_artist_similar(ym_client* client, ym_string_view id,
    const ym_request_options* options, ym_request** out, ym_error* error) YM_NOEXCEPT
{
    return ym::detail::start_api(
        client, [&](auto& spec) { spec.path = {"artists", ym::detail::identifier(id), "similar"}; },
        options, out, error
    );
}
