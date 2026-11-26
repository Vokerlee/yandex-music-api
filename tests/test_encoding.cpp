/*
 * Copyright (c) 2026 Vokerlee <vokerlee@gmail.com>
 *
 * Yandex Music Native is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as published
 * by the Free Software Foundation.
 *
 * Yandex Music Native is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "protocol/encode.hpp"
#include "test_support.hpp"
#include <limits>
using namespace ym::detail;
using namespace ym::detail::wire;
field text(std::string key, std::string value)
{
    return { std::move(key), ym_value(std::move(value)) };
}
int main()
{
    request_spec search;
    search.origin = "https://example.invalid";
    search.path = { "users", "owner/name", "playlists", "kind:one" };
    search.query = { text("text", "Бах + &?"), { "omitted", std::nullopt }, text("empty", ""),
        text("id", "1"), text("id", "2") };
    auto result = encode(search);
    CHECK(result
        && result->url
            == "https://example.invalid/users/owner%2Fname/playlists/"
               "kind%3Aone?text=%D0%91%D0%B0%D1%85%20%2B%20%26%3F&empty=&id=1&id=2");
    CHECK(result->method == "GET" && !result->mutating && result->body.empty());
    request_spec form;
    form.origin = search.origin;
    form.path = { "batch" };
    form.verb = method::post;
    form.encoding = body_kind::form;
    form.body = { text("track-ids", "9007199254740993:42,7"), text("space", "a +b"),
        { "missing", std::nullopt }, { "flag", ym_value(false) } };
    result = encode(form);
    CHECK(result && result->body == "track-ids=9007199254740993%3A42%2C7&space=a+%2Bb&flag=false");
    CHECK(!result->mutating); // POST alone never implies a mutation/retry policy
    CHECK(result->headers[0].second == "application/x-www-form-urlencoded");
    form.body.push_back({ "null", ym_value() });
    CHECK(!encode(form) && encode(form).error().code == YM_INVALID_ARGUMENT);
    request_spec json;
    json.origin = search.origin;
    json.path = { "edit" };
    json.verb = method::post;
    json.encoding = body_kind::json;
    json.mutating = true;
    json.body = { { "missing", std::nullopt }, { "null", ym_value() },
        { "integer", ym_value(uint64_t(18446744073709551615ULL)) }, text("wire-key", "a\n\"b"),
        { "nested", ym_value(ym_value::array { ym_value(false), ym_value(int64_t(-1)) }) } };
    result = encode(json);
    CHECK(result && result->mutating);
    CHECK(result->body
        == "{\"null\":null,\"integer\":18446744073709551615,\"wire-key\":\"a\\n\\\"b\",\"nested\":["
           "false,-1]}");
    json.body.push_back({ "integer", ym_value(int64_t(1)) });
    CHECK(!encode(json));
    json.body.pop_back();
    json.body.push_back({ "bad", ym_value(std::numeric_limits<double>::infinity()) });
    CHECK(!encode(json));
    CHECK(!encode(search, 10) && encode(search, 10).error().code == YM_RESOURCE_LIMIT);
    search.path = { std::string("a\0b", 3) };
    CHECK(!encode(search));
    search.path = { std::string("\xc0\x80", 2) };
    CHECK(!encode(search)); // overlong NUL
    search.path = { std::string("\xed\xa0\x80", 3) };
    CHECK(!encode(search)); // surrogate
    search.path = { std::string("\xf4\x90\x80\x80", 4) };
    CHECK(!encode(search)); // > U+10ffff
    search.path = { "safe" };
    search.origin = "https://user@example.invalid";
    CHECK(!encode(search));
    search.origin = "https://example.invalid/path";
    CHECK(!encode(search));
    search.origin = "http://example.invalid";
    CHECK(!encode(search));
    search.origin = "https://example.invalid";
    search.headers = { { "X-Test", "value\r\nInjected: true" } };
    CHECK(!encode(search));
    search.headers = { { "X-Test", "one" }, { "x-test", "two" } };
    CHECK(!encode(search));
    form.body.clear();
    form.body.push_back(text("difference", "{\"op\":\"insert\"}"));
    result = encode(form);
    CHECK(result && result->body == "difference=%7B%22op%22%3A%22insert%22%7D");
}
