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

#include <memory>
#include <new>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

#include <yandex_music/cpp/error.hpp>

namespace ym {

template<class Tag>
class EntityId {
public:
    [[nodiscard]] static Result<EntityId> create(std::string_view value) noexcept
    {
        return make_id(value);
    }

    [[nodiscard]] std::string_view value() const noexcept
    {
        return value_ ? std::string_view{*value_} : std::string_view{};
    }

    bool operator==(const EntityId& other) const noexcept { return value() == other.value(); }

private:
    explicit EntityId(std::shared_ptr<const std::string> value) noexcept
        : value_{std::move(value)}
    {
    }

    static Result<EntityId> make_id(std::string_view value) noexcept
    {
        ym_error failure;
        ym_error_init(&failure);

        try {
            return EntityId{std::make_shared<const std::string>(value)};
        } catch (const std::bad_alloc&) {
            failure.code = YM_OUT_OF_MEMORY;
        } catch (...) {
            failure.code = YM_INTERNAL_ERROR;
        }

        return std::unexpected(Error{failure});
    }

    std::shared_ptr<const std::string> value_;
};

using TrackId = EntityId<struct TrackIdTag>;
using AlbumId = EntityId<struct AlbumIdTag>;
using ArtistId = EntityId<struct ArtistIdTag>;
using UserId = EntityId<struct UserIdTag>;
using PlaylistKind = EntityId<struct PlaylistKindTag>;

struct TrackReference {
    TrackId id;
    std::optional<AlbumId> album;
};

struct PlaylistKey {
    UserId owner;
    PlaylistKind kind;
};

} // namespace ym
