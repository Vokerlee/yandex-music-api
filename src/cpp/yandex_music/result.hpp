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

#include <yandex_music/error.hpp>
#include <yandex_music/result.h>

namespace ym {

class ValueView {
public:
    explicit ValueView(const ym_value* value = nullptr) noexcept
        : value_{value}
    {
    }

    [[nodiscard]] ym_value_type type() const noexcept { return ym_value_typeof(value_); }

    [[nodiscard]] Result<std::string_view> string() const noexcept
    {
        ym_string_view text{};
        ym_error failure;
        if (ym_value_string(value_, &text, &failure) != YM_OK)
            return std::unexpected(Error{failure});
        return text.size ? std::string_view(text.data, text.size) : std::string_view{};
    }

    [[nodiscard]] Result<uint64_t> unsigned_integer() const noexcept
    {
        uint64_t value{};
        ym_error failure;
        if (ym_value_uint64(value_, &value, &failure) != YM_OK)
            return std::unexpected(Error{failure});
        return value;
    }

    [[nodiscard]] Result<int64_t> integer() const noexcept
    {
        int64_t value{};
        ym_error failure;
        if (ym_value_int64(value_, &value, &failure) != YM_OK)
            return std::unexpected(Error{failure});
        return value;
    }

    [[nodiscard]] Result<double> number() const noexcept
    {
        double value{};
        ym_error failure;
        if (ym_value_double(value_, &value, &failure) != YM_OK)
            return std::unexpected(Error{failure});
        return value;
    }

    [[nodiscard]] Result<bool> boolean() const noexcept
    {
        uint32_t value{};
        ym_error failure;
        if (ym_value_bool(value_, &value, &failure) != YM_OK)
            return std::unexpected(Error{failure});
        return value != 0;
    }

    [[nodiscard]] Result<size_t> size() const noexcept
    {
        size_t value{};
        ym_error failure;
        if (ym_value_size(value_, &value, &failure) != YM_OK)
            return std::unexpected(Error{failure});
        return value;
    }

    [[nodiscard]] Result<ValueView> member(std::string_view name) const noexcept
    {
        const ym_value* value = nullptr;
        ym_error failure;
        const auto status = ym_value_member(value_, {name.data(), name.size()}, &value, &failure);
        if (status != YM_OK && status != YM_NOT_FOUND)
            return std::unexpected(Error{failure});
        return ValueView{value};
    }

    [[nodiscard]] Result<ValueView> at(size_t index) const noexcept
    {
        const ym_value* value = nullptr;
        ym_error failure;
        if (ym_value_at(value_, index, &value, &failure) != YM_OK)
            return std::unexpected(Error{failure});
        return ValueView{value};
    }

private:
    const ym_value* value_{nullptr};
};

class Document {
public:
    Document() noexcept = default;

    explicit Document(ym_result* handle) noexcept
        : handle_{handle}
    {
    }

    Document(const Document&) = delete;
    Document& operator=(const Document&) = delete;

    Document(Document&& other) noexcept
        : handle_{std::exchange(other.handle_, nullptr)}
    {
    }

    Document& operator=(Document&& other) noexcept
    {
        if (this != &other) {
            ym_result_destroy(handle_);
            handle_ = std::exchange(other.handle_, nullptr);
        }
        return *this;
    }

    ~Document() { ym_result_destroy(handle_); }

    [[nodiscard]] ValueView root() const noexcept { return ValueView{ym_result_root(handle_)}; }

    [[nodiscard]] Result<Document> clone() const noexcept
    {
        ym_result* copy = nullptr;
        ym_error failure;
        if (ym_result_clone(handle_, &copy, &failure) != YM_OK)
            return std::unexpected(Error{failure});
        return Document{copy};
    }

    [[nodiscard]] ym_result* native_handle() const noexcept { return handle_; }

    [[nodiscard]] uint32_t http_status() const noexcept { return ym_result_http_status(handle_); }

    [[nodiscard]] std::string_view request_id() const noexcept
    {
        const auto view = ym_result_request_id(handle_);
        return view.size ? std::string_view(view.data, view.size) : std::string_view{};
    }

private:
    ym_result* handle_{nullptr};
};

} // namespace ym
