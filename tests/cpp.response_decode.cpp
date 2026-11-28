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
 * Native scope: C++ response facade conformance.
 */

#include "test_support.hpp"

#include <yandex_music/cpp/yandex_music.hpp>

int main()
{
    auto result = ym::parse_response(R"({"result":{"someKey":"value"}})");

    CHECK(result && *result->root().member("someKey")->string() == "value");

    auto null = ym::parse_response(R"({"result":null})", 200, {.envelope = YM_RESPONSE_RESULT});

    CHECK(null && null->root().type() == YM_VALUE_NULL);

    auto root = ym::parse_response("[false,0]", 200, {.envelope = YM_RESPONSE_ROOT});

    CHECK(root && !*root->root().at(0)->boolean());
    CHECK(!ym::parse_response("[]") && !ym::parse_response("not json"));
}
