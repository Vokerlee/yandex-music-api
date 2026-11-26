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
#include <cstring>
#include <new>
#include <type_traits>
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
    static_assert(!std::is_copy_constructible_v<ym::client>);
    ym_error failure;
    ym_client* raw = reinterpret_cast<ym_client*>(1);
    ym_client_options options;
    ym_client_options_init(&options);
    options.version = 99;
    CHECK(ym_client_create(&options, &raw, &failure) == YM_UNSUPPORTED);
    CHECK(raw == nullptr && failure.code == YM_UNSUPPORTED);
    auto saved = failure;
    CHECK(ym_client_create(nullptr, nullptr, &failure) == YM_INVALID_ARGUMENT);
    CHECK(saved.code == YM_UNSUPPORTED && std::strstr(saved.message, "version"));
    options.version = 1;
    options.struct_size = 1;
    CHECK(ym_client_create(&options, &raw, nullptr) == YM_INVALID_ARGUMENT);
    struct old_options {
        size_t size;
        uint32_t version;
    } old { sizeof(old), 1 };
    CHECK(ym_client_create(reinterpret_cast<const ym_client_options*>(&old), &raw, &failure)
        == YM_OK);
    CHECK(ym_client_language(raw).size == 2);
    ym_client_destroy(raw);
    struct future_options {
        ym_client_options known;
        uint64_t tail;
    } future { };
    ym_client_options_init(&future.known);
    future.known.struct_size = sizeof(future);
    CHECK(ym_client_create(&future.known, &raw, nullptr) == YM_OK);
    ym_client_destroy(raw);
    ym_client_options_init(&options);
    options.token = { nullptr, 4 };
    CHECK(ym_client_create(&options, &raw, nullptr) == YM_INVALID_ARGUMENT);
    options.token = { "a\0b", 3 };
    CHECK(ym_client_create(&options, &raw, nullptr) == YM_INVALID_ARGUMENT);
    char language[] = "en";
    auto client = ym::client::create({ .language = language });
    CHECK(client);
    language[0] = 'x';
    CHECK(client->language() == "en");
    auto moved = std::move(*client);
    CHECK(client->language().empty() && moved.language() == "en");
#if defined(YM_STATIC)
    fail_allocation = true;
    auto no_memory = ym::client::create();
    fail_allocation = false;
    CHECK(!no_memory && no_memory.error().code() == YM_OUT_OF_MEMORY);
#endif
    ym_client_destroy(nullptr);
    CHECK(std::strcmp(ym_status_name(999), "YM_UNKNOWN_STATUS") == 0);
}
