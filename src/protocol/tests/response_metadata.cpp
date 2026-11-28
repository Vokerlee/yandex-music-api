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
 * Native scope: HTTP status and request metadata ownership.
 */

#include "protocol/response.hpp"
#include "test_support.hpp"

#include <cstring>

#include <yandex_music/cpp/yandex_music.hpp>
using namespace ym::detail;

int main()
{
    TransportResponse input;
    input.http_status = 429;
    input.body = "{}";
    input.request_id = "header-id";
    input.retry_after_ms = 1500;
    ym_result* result = nullptr;
    ym_error error;

    CHECK(decode_response(input, &result, &error) == YM_RATE_LIMITED);
    CHECK(error.retry_after_ms == 1500 && std::strcmp(error.request_id, "header-id") == 0);

    input.http_status = 200;
    input.body = R"({"result":true})";

    CHECK(decode_response(input, &result, &error) == YM_OK);

    ym::Document owner(result);

    CHECK(owner.http_status() == 200 && owner.request_id() == "header-id");

    auto copy = owner.clone();
    owner = ym::Document();

    CHECK(copy->request_id() == "header-id");
}
