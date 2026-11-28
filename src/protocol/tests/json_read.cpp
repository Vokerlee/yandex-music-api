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
 * Native scope: JSON parsing and malformed-input conformance.
 */

#include "protocol/json_read.hpp"
#include "test_support.hpp"

#include <yandex_music/cpp/result.hpp>
using namespace ym::detail;

int main()
{
    auto parsed = read_json(R"({"items":[false,null,""]})");

    CHECK(parsed);

    ym::ValueView root(&*parsed);

    CHECK(!*root.member("items")->at(0)->boolean());
    CHECK(root.member("items")->at(1)->type() == YM_VALUE_NULL &&
        root.member("items")->at(2)->string()->empty());
    CHECK(read_json("18446744073709551615")->data.index() == 3);
    CHECK(!read_json("18446744073709551616") && !read_json("1e400"));
    CHECK(!read_json(R"({"a":1,"a":2})") && !read_json("not json"));
    CHECK(!read_json(std::string("\xff\xfe", 2)));
    CHECK(read_json("[]", {.max_bytes = 1}).error().code == YM_RESOURCE_LIMIT);
    CHECK(read_json("[[]]", {.max_depth = 1}).error().code == YM_RESOURCE_LIMIT);
    CHECK(read_json("[1,2]", {.max_nodes = 2}).error().code == YM_RESOURCE_LIMIT);
    CHECK(read_json("[1,2]", {.max_collection_items = 1}).error().code == YM_RESOURCE_LIMIT);
}
