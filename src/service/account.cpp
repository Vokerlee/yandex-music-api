/*
 * Endpoint behavior references MarshalX/yandex-music-api at
 * 9ca132e5c18a72aab779c1fc68c169380a9357d0:
 * yandex_music/_client/account.py and _client/likes.py (account and library).
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

#include <yandex_music/c/account.h>

namespace {

const char* object_type(ym_library_type type)
{
    constexpr std::array types{"track", "artist", "album", "playlist"};
    if (type >= types.size())
        throw std::invalid_argument("Unknown library type");
    return types[type];
}

} // namespace

ym_status YM_CALL ym_account_status(ym_client* client, const ym_request_options* options,
    ym_request** out, ym_error* error) YM_NOEXCEPT
{
    return ym::detail::start_api(
        client, [](auto& spec) { spec.path = {"account", "status"}; }, options, out, error
    );
}

ym_status YM_CALL ym_likes(ym_client* client, ym_string_view user, ym_library_type type,
    const ym_request_options* options, ym_request** out, ym_error* error) YM_NOEXCEPT
{
    return ym::detail::start_api(
        client,
        [&](auto& spec) {
            spec.path = {"users", ym::detail::identifier(user), "likes",
                std::string(object_type(type)) + "s"};
            if (type == YM_LIBRARY_TRACK)
                spec.query = {ym::detail::field("if-modified-since-revision", "0")};
            else if (type == YM_LIBRARY_ARTIST)
                spec.query = {ym::detail::field("with-timestamps", "True")};
            else if (type == YM_LIBRARY_ALBUM)
                spec.query = {ym::detail::field("rich", "True")};
        },
        options, out, error
    );
}

ym_status YM_CALL ym_set_like(ym_client* client, ym_string_view user, ym_library_type type,
    ym_string_view ids, uint32_t liked, const ym_request_options* options, ym_request** out,
    ym_error* error) YM_NOEXCEPT
{
    return ym::detail::start_api(
        client,
        [&](auto& spec) {
            if (liked > 1)
                throw std::invalid_argument("liked must be zero or one");
            const std::string kind = object_type(type);
            spec.path = {"users", ym::detail::identifier(user), "likes", kind + "s",
                liked ? "add-multiple" : "remove"};
            spec.verb = ym::detail::wire::HttpMethod::post;
            spec.mutating = true;
            spec.encoding = ym::detail::wire::BodyKind::form;
            spec.body = {ym::detail::field(kind + "-ids", ym::detail::identifier(ids))};
        },
        options, out, error
    );
}
