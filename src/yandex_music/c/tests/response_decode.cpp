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
 * Native scope: C response ABI and envelope conformance.
 */

#include "test_support.hpp"

#include <cstring>

#include <yandex_music/cpp/yandex_music.hpp>

int main()
{
    const char* body = R"({"result":{"someKey":"value"}})";
    ym_result* result = nullptr;
    ym_error error;

    CHECK(ym_response_parse(200, {body, std::strlen(body)}, nullptr, &result, &error) == YM_OK);

    ym::Document owner(result);

    CHECK(*owner.root().member("someKey")->string() == "value");

    result = reinterpret_cast<ym_result*>(1);

    CHECK(ym_response_parse(200, {nullptr, 1}, nullptr, &result, &error) == YM_INVALID_ARGUMENT &&
        result == nullptr);

    ym_response_options options;
    ym_response_options_init(&options);
    options.version = 9;

    CHECK(ym_response_parse(200, {"{}", 2}, &options, &result, &error) == YM_UNSUPPORTED);

    struct OldOptions {
        size_t size;
        uint32_t version;
    } old{sizeof(old), 1};

    CHECK(ym_response_parse(200, {"{}", 2}, reinterpret_cast<const ym_response_options*>(&old),
              &result, &error) == YM_OK);

    ym_result_destroy(result);

    CHECK(ym_response_parse(401, {"invalid", 7}, nullptr, &result, &error) == YM_UNAUTHORIZED);
}
