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
#include <string_view>
#include <utility>
#include <yandex_music/client.h>
#include <yandex_music/error.hpp>
#include <yandex_music/request.h>

namespace ym
{
struct client_options {
    std::string_view token { };
    std::string_view language = "ru";
    size_t max_response_bytes = 8 * 1024 * 1024;
    size_t max_pending_requests = 64;
};
class client
{
    ym_client* handle_ = nullptr;
    explicit client(ym_client* value) noexcept
        : handle_(value)
    {
    }

public:
    client(const client&) = delete;
    client& operator=(const client&) = delete;
    client(client&& other) noexcept
        : handle_(std::exchange(other.handle_, nullptr))
    {
    }
    client& operator=(client&& other) noexcept
    {
        if (this != &other) {
            ym_client_destroy(handle_);
            handle_ = std::exchange(other.handle_, nullptr);
        }
        return *this;
    }
    ~client() { ym_client_destroy(handle_); }
    [[nodiscard]] static result<client> create(client_options input = { }) noexcept
    {
        ym_client_options options;
        ym_client_options_init(&options);
        options.token = { input.token.data(), input.token.size() };
        options.language = { input.language.data(), input.language.size() };
        options.max_response_bytes = input.max_response_bytes;
        options.max_pending_requests = input.max_pending_requests;
        ym_client* value = nullptr;
        ym_error failure;
        if (ym_client_create(&options, &value, &failure) != YM_OK)
            return std::unexpected(error(failure));
        return client(value);
    }
    [[nodiscard]] std::string_view language() const noexcept
    {
        const auto value = ym_client_language(handle_);
        return value.size ? std::string_view(value.data, value.size) : std::string_view { };
    }
    [[nodiscard]] ym_client* native_handle() const noexcept { return handle_; }
    [[nodiscard]] result<void> poll(uint64_t now_ms, size_t budget = 64) noexcept
    {
        ym_error failure;
        if (ym_client_poll(handle_, now_ms, budget, &failure) != YM_OK)
            return std::unexpected(error(failure));
        return { };
    }
    [[nodiscard]] result<void> close() noexcept
    {
        ym_error failure;
        if (ym_client_close(handle_, &failure) != YM_OK)
            return std::unexpected(error(failure));
        return { };
    }
};
} // namespace ym
