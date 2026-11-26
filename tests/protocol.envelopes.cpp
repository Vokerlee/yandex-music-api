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
 * Native scope: upstream-compatible root and non-null result selection.
 */

#include "protocol/envelope.hpp"
#include "protocol/json_read.hpp"
#include "test_support.hpp"

#include <yandex_music/result.hpp>
using namespace ym::detail;

int main()
{
    auto raw = read_json(R"({"result":{"someKey":"value"}})");

    CHECK(raw);

    auto payload = select_payload(std::move(*raw), EnvelopePolicy::automatic);

    CHECK(payload);
    CHECK(*ym::ValueView(&*payload).member("someKey")->string() == "value");

    raw = read_json(R"({"someKey":"value"})");
    payload = select_payload(std::move(*raw), EnvelopePolicy::automatic);

    CHECK(ym_value_typeof(&*payload) == YM_VALUE_OBJECT);

    raw = read_json(R"({"result":null})");
    payload = select_payload(std::move(*raw), EnvelopePolicy::automatic);

    CHECK(ym_value_typeof(&*payload) == YM_VALUE_OBJECT);

    raw = read_json(R"({"result":null})");
    payload = select_payload(std::move(*raw), EnvelopePolicy::result);

    CHECK(ym_value_typeof(&*payload) == YM_VALUE_NULL);
    CHECK(!select_payload(ym_value(ym_value::Object{}), EnvelopePolicy::result));

    raw = read_json(R"({"result":false})");
    payload = select_payload(std::move(*raw), EnvelopePolicy::automatic);

    CHECK(ym_value_typeof(&*payload) == YM_VALUE_BOOL);
}
