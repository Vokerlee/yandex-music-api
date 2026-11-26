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

#include "core/result.hpp"
#include "test_support.hpp"
#include <limits>
#include <type_traits>
#include <yandex_music/yandex_music.hpp>

int main()
{
    static_assert(!std::is_convertible_v<ym::track_id, ym::album_id>);
    ym_client* client = nullptr;
    CHECK(ym_client_create(nullptr, &client, nullptr) == YM_OK);
    auto* raw = new ym_result { ym_value(
        ym_value::object { { "id", ym_value(std::string("9007199254740993")) },
            { "numeric", ym_value(uint64_t(9007199254740993ULL)) }, { "null", ym_value() },
            { "empty", ym_value(std::string()) }, { "false", ym_value(false) },
            { "zero", ym_value(int64_t(0)) },
            { "array", ym_value(ym_value::array { ym_value(std::string("one")) }) } }) };
    ym::document original(raw);
    ym_client_destroy(client);
    CHECK(*original.root().member("id")->string() == "9007199254740993");
    auto copy = original.clone();
    CHECK(copy);
    original = ym::document();
    auto root = copy->root();
    CHECK(root.type() == YM_VALUE_OBJECT && *root.size() == 7);
    CHECK(root.member("missing")->type() == YM_VALUE_MISSING);
    CHECK(root.member("null")->type() == YM_VALUE_NULL);
    CHECK(root.member("empty")->string()->empty());
    CHECK(!*root.member("false")->boolean());
    CHECK(*root.member("zero")->integer() == 0);
    CHECK(*root.member("numeric")->unsigned_integer() == 9007199254740993ULL);
    CHECK(!root.member("numeric")->number()); // no lossy numeric coercion
    CHECK(*root.member("array")->at(0)->string() == "one");
    CHECK(!root.member("array")->at(1));
    CHECK(!root.at(0));
    const ym_value* missing = reinterpret_cast<const ym_value*>(1);
    ym_error error;
    CHECK(ym_value_member(ym_result_root(copy->native_handle()), { "missing", 7 }, &missing, &error)
        == YM_NOT_FOUND);
    CHECK(missing == nullptr);
    ym_string_view text { "old", 3 };
    CHECK(ym_value_string(nullptr, &text, &error) == YM_SCHEMA_ERROR);
    CHECK(text.data == nullptr && text.size == 0);
    uint64_t value = 42;
    CHECK(ym_value_uint64(nullptr, &value, nullptr) == YM_SCHEMA_ERROR && value == 0);
    CHECK(ym_value_bool(nullptr, nullptr, nullptr) == YM_INVALID_ARGUMENT);
    std::string owned(*root.member("id")->string());
    *copy = ym::document();
    CHECK(owned == "9007199254740993");
    ym_result* out = reinterpret_cast<ym_result*>(1);
    CHECK(ym_result_clone(nullptr, &out, &error) == YM_INVALID_ARGUMENT && out == nullptr);
    CHECK(ym_value_typeof(nullptr) == YM_VALUE_MISSING);
    ym_result_destroy(nullptr);
}
