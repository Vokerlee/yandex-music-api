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

#include <yandex_music/yandex_music.hpp>

#if defined(YM_STATIC)
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

#endif

int main()
{
#if defined(YM_STATIC)
    ym_client* client = nullptr;
    ym_error error;

    fail_allocation = true;
    const auto status = ym_client_create(nullptr, &client, &error);
    auto owner = ym::Client::create();
    fail_allocation = false;

    CHECK(status == YM_OUT_OF_MEMORY && client == nullptr);
    CHECK(error.code == YM_OUT_OF_MEMORY);
    CHECK(!owner && owner.error().code() == YM_OUT_OF_MEMORY);
#endif
}
