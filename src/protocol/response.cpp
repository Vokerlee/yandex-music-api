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
 * Native scope: transport response metadata preservation.
 */

#include "core/error.hpp"
#include "response.hpp"

#include <yandex_music/c/response.h>

namespace ym::detail {

ym_status decode_response(const TransportResponse& response, ym_result** out,
    ym_error* error) noexcept
{
    const auto status = ym_response_parse(response.http_status,
        {response.body.data(), response.body.size()}, nullptr, out, error);
    if (status != YM_OK) {
        if (error) {
            error->retry_after_ms = response.retry_after_ms;
            if (!response.request_id.empty())
                copy_text(error->request_id, response.request_id);
        }
        return status;
    }

    if (response.request_id.empty())
        return status;

    try {
        (*out)->request_id = response.request_id;
    } catch (...) {
        ym_result_destroy(*out);
        *out = nullptr;
        return fail(error, YM_OUT_OF_MEMORY, "Response metadata allocation failed");
    }
    return status;
}

} // namespace ym::detail
