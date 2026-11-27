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
 * Native scope: HTTP and service error classification.
 */

#include "core/error.hpp"
#include "response_error.hpp"

namespace ym::detail {
namespace {

const ym_value* member(const ym_value& root, std::string_view key) noexcept
{
    const ym_value* value = nullptr;
    ym_value_member(&root, {key.data(), key.size()}, &value, nullptr);
    return value;
}

const ym_value* either(const ym_value& root, std::string_view first,
    std::string_view second) noexcept
{
    const auto* value = member(root, first);
    return value ? value : member(root, second);
}

std::string_view text(const ym_value* value) noexcept
{
    ym_string_view view{};
    ym_value_string(value, &view, nullptr);
    return view.size ? std::string_view(view.data, view.size) : std::string_view{};
}

bool has_error(const ym_value* value) noexcept
{
    const auto type = ym_value_typeof(value);
    if (type == YM_VALUE_MISSING || type == YM_VALUE_NULL)
        return false;
    if (type == YM_VALUE_STRING)
        return !text(value).empty();
    if (type == YM_VALUE_BOOL) {
        uint32_t flag = 0;
        ym_value_bool(value, &flag, nullptr);
        return flag != 0;
    }
    return true;
}

ym_status classify(uint32_t status) noexcept
{
    if (status >= 200 && status < 300)
        return YM_OK;
    switch (status) {
    case 400:
        return YM_INVALID_ARGUMENT;
    case 401:
        return YM_UNAUTHORIZED;
    case 403:
        return YM_FORBIDDEN;
    case 404:
        return YM_NOT_FOUND;
    case 409:
        return YM_CONFLICT;
    case 413:
        return YM_RESOURCE_LIMIT;
    case 429:
        return YM_RATE_LIMITED;
    default:
        return YM_SERVICE_ERROR;
    }
}

} // namespace

std::string_view response_request_id(const ym_value& root) noexcept
{
    const auto* invocation = either(root, "invocationInfo", "invocation_info");
    if (!invocation)
        return {};
    const auto* id = either(*invocation, "req-id", "req_id");
    return text(id ? id : member(*invocation, "reqId"));
}

std::optional<ym_error> response_failure(uint32_t http, const ym_value* root) noexcept
{
    auto code = classify(http);
    const auto* service = root ? member(*root, "error") : nullptr;
    if (code == YM_OK && !has_error(service))
        return {};
    if (code == YM_OK)
        code = YM_SERVICE_ERROR;

    ym_error error;
    fail(&error, code, "Service rejected the request");
    error.http_status = http;
    if (root) {
        copy_text(error.server_code, text(service));
        const auto description = text(either(*root, "errorDescription", "error_description"));
        if (!description.empty())
            copy_text(error.message, description);
        copy_text(error.request_id, response_request_id(*root));
    }
    return error;
}

} // namespace ym::detail
