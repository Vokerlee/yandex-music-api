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

#include <cstdlib>
#include <new>
#include <string>
#include <type_traits>
#include <utility>

#include <yandex_music/cpp/yandex_music.hpp>

static bool fail_allocation = false;

void* operator new(std::size_t size)
{
    if (fail_allocation)
        throw std::bad_alloc();
    if (void* value = std::malloc(size ? size : 1))
        return value;
    throw std::bad_alloc();
}

void operator delete(void* value) noexcept
{
    std::free(value);
}

void operator delete(void* value, std::size_t) noexcept
{
    std::free(value);
}

int main()
{
    static_assert(!std::is_convertible_v<ym::TrackId, ym::AlbumId>);
    static_assert(std::is_nothrow_copy_constructible_v<ym::TrackId>);
    static_assert(std::is_nothrow_move_constructible_v<ym::TrackId>);
    static_assert(std::is_nothrow_copy_assignable_v<ym::PlaylistKey>);
    static_assert(noexcept(ym::TrackId::create({})));

    std::string source = "9007199254740993";
    auto id = ym::TrackId::create(source);
    auto album = ym::AlbumId::create("42");
    auto owner = ym::UserId::create("owner");
    auto kind = ym::PlaylistKind::create("kind");
    CHECK(id && album && owner && kind);

    source.clear();
    ym::TrackReference track{*id, *album};
    ym::PlaylistKey playlist{*owner, *kind};
    CHECK(track.id.value() == "9007199254740993" && track.album->value() == "42");
    CHECK(playlist.owner.value() == "owner" && playlist.kind.value() == "kind");

    auto same = ym::TrackId::create(track.id.value());
    CHECK(same && *same == track.id);

    fail_allocation = true;
    auto copy = track.id;
    auto moved = std::move(copy);
    auto failed = ym::TrackId::create("42");
    fail_allocation = false;

    CHECK(moved == track.id);
    CHECK(copy.value().empty());
    CHECK(!failed && failed.error().code() == YM_OUT_OF_MEMORY);
}
