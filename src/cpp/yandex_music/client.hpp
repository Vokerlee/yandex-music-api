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
#include <string_view>

#include <yandex_music/client.h>
#include <yandex_music/error.hpp>
#include <yandex_music/request.h>

namespace ym {

struct ClientOptions {
    std::string_view token{};
    std::string_view language{"ru"};
    size_t max_response_bytes{8 * 1024 * 1024};
    size_t max_pending_requests{64};
};

class Client {
public:
    Client(const Client&) = delete;
    Client& operator=(const Client&) = delete;

    Client(Client&&) noexcept = default;
    Client& operator=(Client&&) noexcept = default;
    ~Client() noexcept = default;

    [[nodiscard]] static Result<Client> create(ClientOptions input = {}) noexcept
    {
        ym_client_options options;
        ym_client_options_init(&options);
        options.token = {input.token.data(), input.token.size()};
        options.language = {input.language.data(), input.language.size()};
        options.max_response_bytes = input.max_response_bytes;
        options.max_pending_requests = input.max_pending_requests;

        ym_client* value = nullptr;
        ym_error failure;
        if (ym_client_create(&options, &value, &failure) != YM_OK)
            return std::unexpected(Error{failure});

        return Client{value};
    }

    [[nodiscard]] std::string_view language() const noexcept
    {
        const auto value = ym_client_language(handle_.get());
        return value.size ? std::string_view(value.data, value.size) : std::string_view{};
    }

    [[nodiscard]] ym_client* native_handle() const noexcept { return handle_.get(); }

    [[nodiscard]] Result<void> poll(uint64_t now_ms, size_t budget = 64) noexcept
    {
        ym_error failure;
        if (ym_client_poll(handle_.get(), now_ms, budget, &failure) != YM_OK)
            return std::unexpected(Error{failure});
        return {};
    }

    [[nodiscard]] Result<void> close() noexcept
    {
        ym_error failure;
        if (ym_client_close(handle_.get(), &failure) != YM_OK)
            return std::unexpected(Error{failure});
        return {};
    }

private:
    struct Deleter {
        void operator()(ym_client* value) const noexcept { ym_client_destroy(value); }
    };

    explicit Client(ym_client* value) noexcept
        : handle_{value}
    {
    }

    std::unique_ptr<ym_client, Deleter> handle_;
};

} // namespace ym
