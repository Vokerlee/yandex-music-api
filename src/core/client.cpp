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

#include "client.hpp"
#include "error.hpp"

#include <cstring>
#include <expected>
#include <memory>
#include <new>

namespace {

bool valid_text(ym_string_view input) noexcept
{
    return (input.data || input.size == 0) &&
        (!input.size || !std::memchr(input.data, 0, input.size));
}

template<class T>
void read_option(const ym_client_options* input, size_t size, size_t offset, T& out) noexcept
{
    if (size >= offset + sizeof(T))
        std::memcpy(&out, reinterpret_cast<const char*>(input) + offset, sizeof(T));
}

void read_options(const ym_client_options* input, size_t size, ym_client_options& out) noexcept
{
    read_option(input, size, offsetof(ym_client_options, token), out.token);
    read_option(input, size, offsetof(ym_client_options, language), out.language);
    read_option(input, size, offsetof(ym_client_options, max_response_bytes),
        out.max_response_bytes);
    read_option(input, size, offsetof(ym_client_options, max_pending_requests),
        out.max_pending_requests);
}

std::expected<ym_client_options, ym_error>
normalize_client_options(const ym_client_options* input) noexcept
{
    ym_client_options options;
    ym_client_options_init(&options);

    if (input) {
        ym_error error;
        size_t size = 0;
        std::memcpy(&size, input, sizeof(size));

        if (size < offsetof(ym_client_options, token)) {
            ym::detail::fail(&error, YM_INVALID_ARGUMENT, "Client options prefix is too small");
            return std::unexpected(error);
        }

        uint32_t version = 0;
        std::memcpy(&version,
            reinterpret_cast<const char*>(input) + offsetof(ym_client_options, version),
            sizeof(version));

        if (version != 1) {
            ym::detail::fail(&error, YM_UNSUPPORTED, "Unsupported client options version");
            return std::unexpected(error);
        }

        read_options(input, size, options);
    }

    if (!valid_text(options.token) || !valid_text(options.language) || options.language.size == 0 ||
        options.max_response_bytes == 0 || options.max_pending_requests == 0) {
        ym_error error;
        ym::detail::fail(&error, YM_INVALID_ARGUMENT, "Invalid client options");
        return std::unexpected(error);
    }

    return options;
}

std::expected<std::unique_ptr<ym_client>, ym_error>
make_client(const ym_client_options& options) noexcept
{
    ym_error error;

    try {
        auto client = std::make_unique<ym_client>();

        if (options.token.size)
            client->token.assign(options.token.data, options.token.size);
        client->language.assign(options.language.data, options.language.size);

        client->max_response_bytes = options.max_response_bytes;
        client->max_pending_requests = options.max_pending_requests;
        client->execution->max_pending = options.max_pending_requests;
        client->execution->max_response_bytes = options.max_response_bytes;

        return client;
    } catch (const std::bad_alloc&) {
        ym::detail::fail(&error, YM_OUT_OF_MEMORY, "Client allocation failed");
    } catch (...) {
        ym::detail::fail(&error, YM_INTERNAL_ERROR, "Client creation failed");
    }

    return std::unexpected(error);
}

} // namespace

void YM_CALL ym_client_options_init(ym_client_options* options) YM_NOEXCEPT
{
    if (!options)
        return;
    *options = {};
    options->struct_size = sizeof(*options);
    options->version = 1;
    options->language = {"ru", 2};
    options->max_response_bytes = 8 * 1024 * 1024;
    options->max_pending_requests = 64;
}

ym_status YM_CALL ym_client_create(const ym_client_options* input, ym_client** out,
    ym_error* error) YM_NOEXCEPT
{
    ym::detail::clear(error);

    if (!out)
        return ym::detail::fail(error, YM_INVALID_ARGUMENT, "Client output is required");

    *out = nullptr;
    auto options = normalize_client_options(input);

    if (!options) {
        if (error)
            *error = options.error();
        return options.error().code;
    }

    auto client = make_client(*options);

    if (!client) {
        if (error)
            *error = client.error();
        return client.error().code;
    }

    *out = client->release();
    return YM_OK;
}

ym_string_view YM_CALL ym_client_language(const ym_client* client) YM_NOEXCEPT
{
    if (!client)
        return {};
    return {client->language.data(), client->language.size()};
}
