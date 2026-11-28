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
 * Native scope: bounded HTTP and service error conformance.
 */

#include "core/text.hpp"
#include "protocol/json_read.hpp"
#include "protocol/response_error.hpp"
#include "test_support.hpp"

#include <cstring>
using namespace ym::detail;

int main()
{
    auto root = read_json(
        R"({"error":"example_error","errorDescription":"details","invocationInfo":{"req-id":"request-1"}})"
    );
    auto failure = response_failure(200, &*root);

    CHECK(failure && failure->code == YM_SERVICE_ERROR);
    CHECK(std::strcmp(failure->server_code, "example_error") == 0 &&
        std::strcmp(failure->request_id, "request-1") == 0);

    for (auto [http, code] : {
             std::pair{401U, YM_UNAUTHORIZED},
             {403U, YM_FORBIDDEN   },
             {404U, YM_NOT_FOUND   },
             {409U, YM_CONFLICT    },
             {429U, YM_RATE_LIMITED}
    })

        CHECK(response_failure(http, nullptr)->code == static_cast<ym_status>(code));
    CHECK(!response_failure(200, nullptr));

    std::string description(254, 'x');
    description += "Бах";
    ym_value value(ym_value::Object{
        {"error",             ym_value(std::string("bad"))},
        {"error_description", ym_value(description)       }
    });
    failure = response_failure(400, &value);

    CHECK(failure && std::strlen(failure->message) <= 255 && valid_utf8(failure->message));
}
