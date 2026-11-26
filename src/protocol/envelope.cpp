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
 * Native scope: explicit root/result envelope selection.
 */

#include "core/error.hpp"
#include "envelope.hpp"

namespace ym::detail {

std::expected<ym_value, ym_error> select_payload(ym_value root, EnvelopePolicy policy) noexcept
{
    ym_error error;
    if (policy == EnvelopePolicy::root)
        return root;
    if (!std::holds_alternative<ym_value::Object>(root.data)) {
        fail(&error, YM_SCHEMA_ERROR, "Response envelope is not an object");
        return std::unexpected(error);
    }
    const ym_value* payload = nullptr;
    const auto status = ym_value_member(&root, {"result", 6}, &payload, nullptr);
    if (policy == EnvelopePolicy::result && status == YM_NOT_FOUND) {
        fail(&error, YM_SCHEMA_ERROR, "Required result envelope is missing");
        return std::unexpected(error);
    }
    if (payload && (policy == EnvelopePolicy::result || ym_value_typeof(payload) != YM_VALUE_NULL))
        return std::move(*const_cast<ym_value*>(payload));
    return root;
}

} // namespace ym::detail
