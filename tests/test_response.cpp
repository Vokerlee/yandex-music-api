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
 * Native changes: bounded SAX mapping, explicit envelope policies, typed HTTP
 * errors, duplicate-key checks and rejection of service errors in HTTP 200.
 */

#include "core/text.hpp"
#include "protocol/response.hpp"
#include "test_support.hpp"
#include <cstring>
#include <limits>
#include <yandex_music/yandex_music.hpp>

int main()
{
    auto envelope = ym::parse_response(
        R"({"result":{"someKey":"value","nested-key":{"innerKey":2}},"invocationInfo":{"req-id":"request-1"}})");
    CHECK(envelope && envelope->http_status() == 200 && envelope->request_id() == "request-1");
    CHECK(*envelope->root().member("someKey")->string() == "value");
    CHECK(*envelope->root().member("nested-key")->member("innerKey")->unsigned_integer() == 2);
    CHECK(envelope->root().member("unknown")->type() == YM_VALUE_MISSING);
    auto root = ym::parse_response(R"({"someKey":"value"})");
    CHECK(root && *root->root().member("someKey")->string() == "value");
    auto null = ym::parse_response(R"({"result":null,"extra":true})");
    CHECK(null && null->root().member("result")->type() == YM_VALUE_NULL);
    auto explicit_null
        = ym::parse_response(R"({"result":null})", 200, { .envelope = YM_RESPONSE_RESULT });
    CHECK(explicit_null && explicit_null->root().type() == YM_VALUE_NULL);
    CHECK(!ym::parse_response("{}", 200, { .envelope = YM_RESPONSE_RESULT }));
    CHECK(!ym::parse_response("[]"));
    auto array = ym::parse_response(R"([false,0,""])", 200, { .envelope = YM_RESPONSE_ROOT });
    CHECK(
        array && !*array->root().at(0)->boolean() && *array->root().at(1)->unsigned_integer() == 0);
    CHECK(array->root().at(2)->string()->empty());
    CHECK(ym::parse_response("", 204)->root().type() == YM_VALUE_NULL);
    CHECK(!ym::parse_response(""));
    auto numbers = ym::parse_response(
        R"({"result":{"large":18446744073709551615,"negative":-9223372036854775808,"real":1.5}})");
    CHECK(numbers && *numbers->root().member("large")->unsigned_integer() == UINT64_MAX);
    CHECK(*numbers->root().member("negative")->integer() == INT64_MIN);
    CHECK(*numbers->root().member("real")->number() == 1.5);
    CHECK(!ym::parse_response(R"({"result":18446744073709551616})"));
    CHECK(!ym::parse_response(R"({"result":1e400})"));
    CHECK(!ym::parse_response(R"({"result":1,"result":2})"));
    CHECK(!ym::parse_response(std::string("\xff\xfe", 2)));
    CHECK(!ym::parse_response("not json"));
    auto nul_text = ym::parse_response(R"({"result":"a\u0000b"})");
    CHECK(nul_text && nul_text->root().string()->size() == 3);
    CHECK(!ym::parse_response(R"({"result":{"nested":true}})", 200, { .max_depth = 1 }));
    CHECK(ym::parse_response("{\"result\":1}", 200, { .max_nodes = 1 }).error().code()
        == YM_RESOURCE_LIMIT);
    CHECK(
        ym::parse_response("{\"result\":[1,2]}", 200, { .max_collection_items = 1 }).error().code()
        == YM_RESOURCE_LIMIT);
    CHECK(ym::parse_response("{\"result\":1}", 200, { .max_bytes = 4 }).error().code()
        == YM_RESOURCE_LIMIT);
    const auto failed = ym::parse_response(
        R"({"error":"example_error","errorDescription":"example description","invocationInfo":{"req-id":"failed-1"}})");
    CHECK(!failed && failed.error().code() == YM_SERVICE_ERROR);
    CHECK(failed.error().message() == "example description");
    CHECK(std::strcmp(failed.error().native().server_code, "example_error") == 0);
    CHECK(std::strcmp(failed.error().native().request_id, "failed-1") == 0);
    for (auto [http, expected] : { std::pair { 401U, YM_UNAUTHORIZED }, { 403U, YM_FORBIDDEN },
             { 404U, YM_NOT_FOUND }, { 409U, YM_CONFLICT }, { 429U, YM_RATE_LIMITED } }) {
        auto error = ym::parse_response("not JSON", http);
        CHECK(!error && error.error().code() == static_cast<ym_status>(expected)
            && error.error().native().http_status == http);
    }
    const std::string description(254, 'x');
    auto truncated = ym::parse_response(
        "{\"error\":\"bad\",\"error_description\":\"" + description + "Бах\"}", 400);
    CHECK(!truncated && truncated.error().message().size() <= 255
        && ym::detail::valid_utf8(truncated.error().message()));
    ym_result* raw = reinterpret_cast<ym_result*>(1);
    ym_error error;
    CHECK(ym_response_parse(200, { nullptr, 1 }, nullptr, &raw, &error) == YM_INVALID_ARGUMENT
        && raw == nullptr);
    ym_response_options options;
    ym_response_options_init(&options);
    options.version = 9;
    CHECK(ym_response_parse(200, { "{}", 2 }, &options, &raw, nullptr) == YM_UNSUPPORTED);
    struct old_options {
        size_t size;
        uint32_t version;
    } old { sizeof(old), 1 };
    CHECK(ym_response_parse(
              200, { "{}", 2 }, reinterpret_cast<const ym_response_options*>(&old), &raw, &error)
        == YM_OK);
    ym_result_destroy(raw);
    ym::detail::transport_response response;
    response.http_status = 429;
    response.body = "{}";
    response.retry_after_ms = 1500;
    response.request_id = "header-id";
    CHECK(ym::detail::decode_response(response, &raw, &error) == YM_RATE_LIMITED && raw == nullptr);
    CHECK(error.retry_after_ms == 1500 && std::strcmp(error.request_id, "header-id") == 0);
    response.http_status = 200;
    response.body = "{\"result\":true}";
    CHECK(ym::detail::decode_response(response, &raw, &error) == YM_OK);
    ym::document owned(raw);
    CHECK(owned.request_id() == "header-id" && *owned.root().boolean());
}
