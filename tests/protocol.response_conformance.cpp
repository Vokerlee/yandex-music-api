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
 * Native scope: response precision, drift and limit regression cases.
 */

#include "test_support.hpp"

#include <limits>

#include <yandex_music/yandex_music.hpp>

int main()
{
    auto value = ym::parse_response(R"({"result":{"camelCase":1,"nested-key":{"innerKey":2}}})");

    CHECK(value);
    CHECK(*value->root().member("nested-key")->member("innerKey")->unsigned_integer() == 2);
    CHECK(value->root().member("unknown")->type() == YM_VALUE_MISSING);

    CHECK(ym::parse_response(R"({"result":"a\u0000b"})")->root().string()->size() == 3);

    CHECK(*ym::parse_response(R"({"result":18446744073709551615})")->root().unsigned_integer() ==
        UINT64_MAX);
    CHECK(!ym::parse_response(R"({"result":18446744073709551616})"));

    CHECK(ym::parse_response("", 204)->root().type() == YM_VALUE_NULL);
    CHECK(!ym::parse_response(""));

    CHECK(
        ym::parse_response(R"({"result":{"nested":true}})", 200, {.max_depth = 1}).error().code() ==
        YM_RESOURCE_LIMIT
    );
    CHECK(ym::parse_response(R"({"result":1})", 200, {.max_nodes = 1}).error().code() ==
        YM_RESOURCE_LIMIT);
    CHECK(ym::parse_response(R"({"result":[1,2]})", 200, {.max_collection_items = 1})
              .error()
              .code() == YM_RESOURCE_LIMIT);
}
