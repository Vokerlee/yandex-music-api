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
 * Native scope: bounded SAX mapping without response envelope or HTTP policy.
 */

#include "core/error.hpp"
#include "core/text.hpp"
#include "json_read.hpp"

#include <cmath>
#include <set>

#include <nlohmann/json.hpp>

namespace ym::detail {
namespace {

struct Sax final : nlohmann::json_sax<nlohmann::json> {
    struct Frame {
        ym_value value;
        std::string key;
        bool has_key = false;
        std::set<std::string> keys;
    };

    JsonLimits options;
    std::vector<Frame> stack;
    ym_value root;
    bool has_root = false;
    size_t nodes = 0;
    ym_status failure = YM_SCHEMA_ERROR;
    const char* message = "Invalid JSON response";

    explicit Sax(JsonLimits limits)
        : options(limits)
    {
    }

    bool reject(const char* text, ym_status code = YM_SCHEMA_ERROR) noexcept
    {
        message = text;
        failure = code;
        return false;
    }

    bool count()
    {
        return ++nodes <= options.max_nodes ||
            reject("Response node limit exceeded", YM_RESOURCE_LIMIT);
    }

    bool emit(ym_value value, bool count_node = true)
    {
        if (count_node && !count())
            return false;
        if (stack.empty()) {
            if (has_root)
                return reject("Multiple response roots");
            root = std::move(value);
            has_root = true;
            return true;
        }
        auto& parent = stack.back();
        if (auto* array = std::get_if<ym_value::Array>(&parent.value.data)) {
            if (array->size() >= options.max_collection_items)
                return reject("Response collection limit exceeded", YM_RESOURCE_LIMIT);
            array->push_back(std::move(value));
        } else {
            auto& object = std::get<ym_value::Object>(parent.value.data);
            if (!parent.has_key)
                return reject("Response object value has no key");
            if (object.size() >= options.max_collection_items)
                return reject("Response collection limit exceeded", YM_RESOURCE_LIMIT);
            object.emplace_back(std::move(parent.key), std::move(value));
            parent.has_key = false;
        }
        return true;
    }

    bool null() override { return emit(ym_value()); }

    bool boolean(bool value) override { return emit(ym_value(value)); }

    bool number_integer(number_integer_t value) override { return emit(ym_value(int64_t(value))); }

    bool number_unsigned(number_unsigned_t value) override
    {
        return emit(ym_value(uint64_t(value)));
    }

    bool number_float(number_float_t value, const string_t& token) override
    {
        if (!std::isfinite(value) || token.find_first_of(".eE") == std::string::npos)
            return reject("Response number is out of its exact supported range");
        return emit(ym_value(double(value)));
    }

    bool string(string_t& value) override { return emit(ym_value(std::move(value))); }

    bool binary(binary_t&) override { return reject("Binary data is not JSON"); }

    bool begin(bool object, size_t elements)
    {
        if (stack.size() >= options.max_depth)
            return reject("Response depth limit exceeded", YM_RESOURCE_LIMIT);
        if (elements != size_t(-1) && elements > options.max_collection_items)
            return reject("Response collection limit exceeded", YM_RESOURCE_LIMIT);
        if (!count())
            return false;
        Frame next;
        next.value = object ? ym_value(ym_value::Object{}) : ym_value(ym_value::Array{});
        stack.push_back(std::move(next));
        return true;
    }

    bool start_object(size_t elements) override { return begin(true, elements); }

    bool start_array(size_t elements) override { return begin(false, elements); }

    bool key(string_t& value) override
    {
        auto& parent = stack.back();
        if (!parent.keys.insert(value).second)
            return reject("Duplicate response key");
        if (parent.keys.size() > options.max_collection_items)
            return reject("Response collection limit exceeded", YM_RESOURCE_LIMIT);
        parent.key = std::move(value);
        parent.has_key = true;
        return true;
    }

    bool end_object() override
    {
        auto value = std::move(stack.back().value);
        stack.pop_back();
        return emit(std::move(value), false);
    }

    bool end_array() override { return end_object(); }

    bool parse_error(size_t, const std::string&, const nlohmann::detail::exception&) override
    {
        return reject("Invalid JSON response");
    }
};

} // namespace

std::expected<ym_value, ym_error> read_json(std::string_view bytes, JsonLimits options) noexcept
{
    auto failure = [](ym_status code, const char* message) -> std::unexpected<ym_error> {
        ym_error error;
        fail(&error, code, message);
        return std::unexpected(error);
    };
    if (!options.max_bytes || !options.max_depth || options.max_depth > 256 || !options.max_nodes ||
        !options.max_collection_items)
        return failure(YM_INVALID_ARGUMENT, "Invalid JSON limits");
    if (bytes.size() > options.max_bytes)
        return failure(YM_RESOURCE_LIMIT, "Response byte limit exceeded");
    if (bytes.empty() || !valid_utf8(bytes))
        return failure(YM_SCHEMA_ERROR, "Response is empty or not UTF-8");
    try {
        Sax parser(options);
        if (!nlohmann::json::sax_parse(bytes.begin(), bytes.end(), &parser) || !parser.has_root)
            return failure(parser.failure, parser.message);
        return std::move(parser.root);
    } catch (const std::bad_alloc&) {
        return failure(YM_OUT_OF_MEMORY, "JSON allocation failed");
    } catch (...) {
        return failure(YM_SCHEMA_ERROR, "Invalid JSON response");
    }
}

} // namespace ym::detail
