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

#pragma once
#include <optional>
#include <string>
#include <string_view>
#include <utility>

namespace ym
{
template <class Tag> class entity_id
{
    std::string value_;

public:
    explicit entity_id(std::string value)
        : value_(std::move(value))
    {
    }
    [[nodiscard]] std::string_view value() const noexcept { return value_; }
    bool operator==(const entity_id&) const = default;
};
using track_id = entity_id<struct track_id_tag>;
using album_id = entity_id<struct album_id_tag>;
using artist_id = entity_id<struct artist_id_tag>;
using user_id = entity_id<struct user_id_tag>;
struct track_reference {
    track_id id;
    std::optional<album_id> album;
};
struct playlist_key {
    user_id owner;
    std::string kind;
};
} // namespace ym
