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
#include <memory>
#include <new>

namespace
{
bool valid(ym_string_view input) noexcept
{
    return (input.data || input.size == 0)
        && (!input.size || !std::memchr(input.data, 0, input.size));
}
void read_options(const ym_client_options* input, size_t size, ym_client_options& out) noexcept
{
#define YM_READ(field)                                                                             \
    do {                                                                                           \
        if (size >= offsetof(ym_client_options, field) + sizeof(out.field))                        \
            std::memcpy(&out.field,                                                                \
                reinterpret_cast<const char*>(input) + offsetof(ym_client_options, field),         \
                sizeof(out.field));                                                                \
    } while (false)
    YM_READ(token);
    YM_READ(language);
    YM_READ(max_response_bytes);
    YM_READ(max_pending_requests);
#undef YM_READ
}
} // namespace

void YM_CALL ym_client_options_init(ym_client_options* options)
{
    if (!options)
        return;
    *options = { };
    options->struct_size = sizeof(*options);
    options->version = 1;
    options->language = { "ru", 2 };
    options->max_response_bytes = 8 * 1024 * 1024;
    options->max_pending_requests = 64;
}

ym_status YM_CALL ym_client_create(const ym_client_options* input, ym_client** out, ym_error* error)
{
    using namespace ym::detail;
    clear(error);
    if (!out)
        return fail(error, YM_INVALID_ARGUMENT, "Client output is required");
    *out = nullptr;
    try {
        ym_client_options options;
        ym_client_options_init(&options);
        if (input) {
            size_t size = 0;
            std::memcpy(&size, input, sizeof(size));
            if (size < offsetof(ym_client_options, token))
                return fail(error, YM_INVALID_ARGUMENT, "Client options prefix is too small");
            uint32_t version = 0;
            std::memcpy(&version,
                reinterpret_cast<const char*>(input) + offsetof(ym_client_options, version),
                sizeof(version));
            if (version != 1)
                return fail(error, YM_UNSUPPORTED, "Unsupported client options version");
            read_options(input, size, options);
        }
        if (!valid(options.token) || !valid(options.language) || options.language.size == 0
            || options.max_response_bytes == 0 || options.max_pending_requests == 0)
            return fail(error, YM_INVALID_ARGUMENT, "Invalid client options");
        auto client = std::make_unique<ym_client>();
        if (options.token.size)
            client->token.assign(options.token.data, options.token.size);
        client->language.assign(options.language.data, options.language.size);
        client->max_response_bytes = options.max_response_bytes;
        client->max_pending_requests = options.max_pending_requests;
        *out = client.release();
        return YM_OK;
    } catch (const std::bad_alloc&) {
        return fail(error, YM_OUT_OF_MEMORY, "Client allocation failed");
    } catch (...) {
        return fail(error, YM_INTERNAL_ERROR, "Client creation failed");
    }
}

ym_string_view YM_CALL ym_client_language(const ym_client* client)
{
    if (!client)
        return { };
    return { client->language.data(), client->language.size() };
}
