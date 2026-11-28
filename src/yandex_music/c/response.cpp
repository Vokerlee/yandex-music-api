/*
 * Adapted from MarshalX/yandex-music-api at
 * 9ca132e5c18a72aab779c1fc68c169380a9357d0:
 * yandex_music/utils/request_base.py, yandex_music/utils/response.py,
 * tests/test_request_base.py (envelope semantics and conformance cases).
 * Copyright (C) 2019-2026 Ilya (Marshal) <https://github.com/MarshalX>
 * Copyright (c) 2026 Vokerlee <vokerlee@gmail.com>
 *
 * This file is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3 as published
 * by the Free Software Foundation.
 * This file is distributed WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSES/MarshalX-yandex-music-LGPL-3.0.txt and LICENSE for the license texts.
 * Native scope: C response decoding over the bounded reader and explicit policy components.
 */

#include "core/error.hpp"
#include "protocol/envelope.hpp"
#include "protocol/json_read.hpp"
#include "protocol/response_error.hpp"

#include <cstring>
#include <memory>
#include <new>

#include <yandex_music/c/response.h>

namespace {

template<class T>
void read_option(const ym_response_options* input, size_t size, size_t offset, T& out) noexcept
{
    if (size >= offset + sizeof(T))
        std::memcpy(&out, reinterpret_cast<const char*>(input) + offset, sizeof(T));
}

bool normalize(const ym_response_options* input, ym_response_options& options,
    ym_error* error) noexcept
{
    ym_response_options_init(&options);
    if (!input)
        return true;
    size_t size = 0;
    std::memcpy(&size, input, sizeof(size));
    if (size < offsetof(ym_response_options, max_bytes)) {
        ym::detail::fail(error, YM_INVALID_ARGUMENT, "Response options prefix is too small");
        return false;
    }
    uint32_t version = 0;
    std::memcpy(&version,
        reinterpret_cast<const char*>(input) + offsetof(ym_response_options, version),
        sizeof(version));
    if (version != 1) {
        ym::detail::fail(error, YM_UNSUPPORTED, "Unsupported response options version");
        return false;
    }
    read_option(input, size, offsetof(ym_response_options, max_bytes), options.max_bytes);
    read_option(input, size, offsetof(ym_response_options, max_depth), options.max_depth);
    read_option(input, size, offsetof(ym_response_options, max_nodes), options.max_nodes);
    read_option(input, size, offsetof(ym_response_options, max_collection_items),
        options.max_collection_items);
    read_option(input, size, offsetof(ym_response_options, envelope), options.envelope);
    return true;
}

std::unique_ptr<ym_result> parse_payload(uint32_t http, ym_string_view body,
    const ym_response_options& options, ym_error* error) noexcept
{
    using namespace ym::detail;
    ym_error detail;
    try {
        auto result = std::make_unique<ym_result>();
        result->http_status = http;
        if (!body.size && (http == 204 || http == 205) && options.envelope != YM_RESPONSE_RESULT) {
            return result;
        }
        auto parsed =
            read_json(body.size ? std::string_view(body.data, body.size) : std::string_view{},
                {options.max_bytes, options.max_depth, options.max_nodes,
                    options.max_collection_items});
        if (!parsed) {
            detail = response_failure(http, nullptr).value_or(parsed.error());
            detail.http_status = http;
            if (error)
                *error = detail;
            return nullptr;
        }
        if (auto failure = response_failure(http, &*parsed)) {
            if (error)
                *error = *failure;
            return nullptr;
        }
        result->request_id = response_request_id(*parsed);
        auto payload =
            select_payload(std::move(*parsed), static_cast<EnvelopePolicy>(options.envelope));
        if (!payload) {
            if (error) {
                *error = payload.error();
                error->http_status = http;
            }
            return nullptr;
        }
        result->root = std::move(*payload);
        return result;
    } catch (const std::bad_alloc&) {
        fail(error, YM_OUT_OF_MEMORY, "Response allocation failed");
    } catch (...) {
        fail(error, YM_SCHEMA_ERROR, "Response could not be decoded");
    }
    return nullptr;
}

} // namespace

void YM_CALL ym_response_options_init(ym_response_options* options) YM_NOEXCEPT
{
    if (!options)
        return;
    *options = {};
    options->struct_size = sizeof(*options);
    options->version = 1;
    options->max_bytes = 8 * 1024 * 1024;
    options->max_depth = 64;
    options->max_nodes = 100000;
    options->max_collection_items = 100000;
    options->envelope = YM_RESPONSE_AUTO;
}

ym_status YM_CALL ym_response_parse(uint32_t http, ym_string_view body,
    const ym_response_options* input, ym_result** out, ym_error* error) YM_NOEXCEPT
{
    using namespace ym::detail;
    clear(error);
    if (!out)
        return fail(error, YM_INVALID_ARGUMENT, "Response output is required");
    *out = nullptr;
    if ((!body.data && body.size) || http < 100 || http > 599)
        return fail(error, YM_INVALID_ARGUMENT, "Invalid response input");
    ym_response_options options;
    ym_error detail;
    if (!normalize(input, options, &detail)) {
        if (error)
            *error = detail;
        return detail.code;
    }
    if (!options.max_bytes || !options.max_depth || options.max_depth > 256 || !options.max_nodes ||
        !options.max_collection_items || options.envelope > YM_RESPONSE_ROOT)
        return fail(error, YM_INVALID_ARGUMENT, "Invalid response limits or envelope policy");
    ym_error failure;
    auto result = parse_payload(http, body, options, &failure);
    if (!result) {
        if (error)
            *error = failure;
        return failure.code;
    }

    *out = result.release();
    return YM_OK;
}
