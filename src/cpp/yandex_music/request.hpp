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

#include <utility>

#include <yandex_music/request.h>
#include <yandex_music/result.hpp>

namespace ym {

[[nodiscard]] inline uint64_t monotonic_ms() noexcept
{
    return ym_monotonic_ms();
}

class Request {
public:
    explicit Request(ym_request* handle = nullptr) noexcept
        : handle_{handle}
    {
    }

    Request(const Request&) = delete;
    Request& operator=(const Request&) = delete;

    Request(Request&& other) noexcept
        : handle_{std::exchange(other.handle_, nullptr)}
    {
    }

    Request& operator=(Request&& other) noexcept
    {
        if (this != &other) {
            ym_request_destroy(handle_);
            handle_ = std::exchange(other.handle_, nullptr);
        }
        return *this;
    }

    ~Request() { ym_request_destroy(handle_); }

    [[nodiscard]] ym_request_state state() const noexcept { return ym_request_get_state(handle_); }

    [[nodiscard]] ym_status status() const noexcept { return ym_request_get_status(handle_); }

    [[nodiscard]] Result<void> cancel() noexcept
    {
        ym_error failure;
        if (ym_request_cancel(handle_, &failure) != YM_OK)
            return std::unexpected(Error{failure});
        return {};
    }

    [[nodiscard]] Result<Document> take_result() noexcept
    {
        ym_result* value = nullptr;
        ym_error failure;
        if (ym_request_take_result(handle_, &value, &failure) != YM_OK)
            return std::unexpected(Error{failure});
        return Document{value};
    }

    [[nodiscard]] ym_request* native_handle() const noexcept { return handle_; }

private:
    ym_request* handle_{nullptr};
};

} // namespace ym
