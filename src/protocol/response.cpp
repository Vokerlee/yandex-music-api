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

#include "response.hpp"
#include "core/error.hpp"
#include "core/text.hpp"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <new>
#include <nlohmann/json.hpp>
#include <set>
#include <yandex_music/response.h>

namespace ym::detail
{
namespace
{
struct sax final : nlohmann::json_sax<nlohmann::json> {
    struct frame {
        ym_value value;
        std::string key;
        bool has_key = false;
        std::set<std::string> keys;
    };
    ym_response_options options;
    std::vector<frame> stack;
    ym_value root;
    bool has_root = false;
    size_t nodes = 0;
    ym_status failure = YM_SCHEMA_ERROR;
    const char* message = "Invalid JSON response";
    explicit sax(ym_response_options limits)
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
        return ++nodes <= options.max_nodes
            || reject("Response node limit exceeded", YM_RESOURCE_LIMIT);
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
        if (auto* array = std::get_if<ym_value::array>(&parent.value.data)) {
            if (array->size() >= options.max_collection_items)
                return reject("Response collection limit exceeded", YM_RESOURCE_LIMIT);
            array->push_back(std::move(value));
        } else {
            auto& object = std::get<ym_value::object>(parent.value.data);
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
        frame next;
        next.value = object ? ym_value(ym_value::object { }) : ym_value(ym_value::array { });
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
const ym_value* member(const ym_value& value, std::string_view key) noexcept
{
    if (const auto* object = std::get_if<ym_value::object>(&value.data))
        for (const auto& [name, child] : *object)
            if (name == key)
                return &child;
    return nullptr;
}
const std::string* text(const ym_value* value) noexcept
{
    return value ? std::get_if<std::string>(&value->data) : nullptr;
}
const ym_value* alternative(
    const ym_value& value, std::string_view first, std::string_view second) noexcept
{
    const auto* found = member(value, first);
    return found ? found : member(value, second);
}
ym_status http_error(uint32_t status) noexcept
{
    if (status >= 200 && status < 300)
        return YM_OK;
    switch (status) {
    case 400:
        return YM_INVALID_ARGUMENT;
    case 401:
        return YM_UNAUTHORIZED;
    case 403:
        return YM_FORBIDDEN;
    case 404:
        return YM_NOT_FOUND;
    case 409:
        return YM_CONFLICT;
    case 413:
        return YM_RESOURCE_LIMIT;
    case 429:
        return YM_RATE_LIMITED;
    default:
        return YM_SERVICE_ERROR;
    }
}
bool has_error(const ym_value* value) noexcept
{
    if (!value || std::holds_alternative<std::monostate>(value->data))
        return false;
    if (const auto* item = std::get_if<std::string>(&value->data))
        return !item->empty();
    if (const auto* item = std::get_if<bool>(&value->data))
        return *item;
    return true;
}
std::string_view request_id(const ym_value& root) noexcept
{
    const auto* invocation = alternative(root, "invocationInfo", "invocation_info");
    if (!invocation)
        return { };
    const auto* id = alternative(*invocation, "req-id", "req_id");
    if (!id)
        id = member(*invocation, "reqId");
    const auto* value = text(id);
    return value ? std::string_view(*value) : std::string_view { };
}
} // namespace

ym_status decode_response(
    const transport_response& response, ym_result** out, ym_error* error) noexcept
{
    const auto status = ym_response_parse(
        response.http_status, { response.body.data(), response.body.size() }, nullptr, out, error);
    if (error && status != YM_OK) {
        error->retry_after_ms = response.retry_after_ms;
        if (!response.request_id.empty())
            copy_text(error->request_id, response.request_id);
    }
    if (status == YM_OK && *out && !response.request_id.empty()) {
        try {
            (*out)->request_id = response.request_id;
        } catch (...) {
            ym_result_destroy(*out);
            *out = nullptr;
            return fail(error, YM_OUT_OF_MEMORY, "Response metadata allocation failed");
        }
    }
    return status;
}
} // namespace ym::detail

void YM_CALL ym_response_options_init(ym_response_options* options)
{
    if (!options)
        return;
    *options = { };
    options->struct_size = sizeof(*options);
    options->version = 1;
    options->max_bytes = 8 * 1024 * 1024;
    options->max_depth = 64;
    options->max_nodes = 100000;
    options->max_collection_items = 100000;
    options->envelope = YM_RESPONSE_AUTO;
}
ym_status YM_CALL ym_response_parse(uint32_t http_status, ym_string_view body,
    const ym_response_options* input, ym_result** out, ym_error* error)
{
    using namespace ym::detail;
    clear(error);
    if (!out)
        return fail(error, YM_INVALID_ARGUMENT, "Response output is required");
    *out = nullptr;
    if ((!body.data && body.size) || http_status < 100 || http_status > 599)
        return fail(error, YM_INVALID_ARGUMENT, "Invalid response input");
    try {
        ym_response_options options;
        ym_response_options_init(&options);
        if (input) {
            size_t size = 0;
            std::memcpy(&size, input, sizeof(size));
            if (size < offsetof(ym_response_options, max_bytes))
                return fail(error, YM_INVALID_ARGUMENT, "Response options prefix is too small");
            uint32_t version = 0;
            std::memcpy(&version,
                reinterpret_cast<const char*>(input) + offsetof(ym_response_options, version),
                sizeof(version));
            if (version != 1)
                return fail(error, YM_UNSUPPORTED, "Unsupported response options version");
#define YM_READ(field)                                                                             \
    do {                                                                                           \
        if (size >= offsetof(ym_response_options, field) + sizeof(options.field))                  \
            std::memcpy(&options.field,                                                            \
                reinterpret_cast<const char*>(input) + offsetof(ym_response_options, field),       \
                sizeof(options.field));                                                            \
    } while (false)
            YM_READ(max_bytes);
            YM_READ(max_depth);
            YM_READ(max_nodes);
            YM_READ(max_collection_items);
            YM_READ(envelope);
#undef YM_READ
        }
        if (!options.max_bytes || !options.max_depth || options.max_depth > 256
            || !options.max_nodes || !options.max_collection_items
            || options.envelope > YM_RESPONSE_ROOT)
            return fail(error, YM_INVALID_ARGUMENT, "Invalid response limits or envelope policy");
        const auto http_failure = http_error(http_status);
        auto response_fail = [&](ym_status code, const char* message) {
            const auto result = fail(error, http_failure == YM_OK ? code : http_failure, message);
            if (error)
                error->http_status = http_status;
            return result;
        };
        if (body.size > options.max_bytes)
            return response_fail(YM_RESOURCE_LIMIT, "Response byte limit exceeded");
        auto result = std::make_unique<ym_result>();
        result->http_status = http_status;
        if (!body.size && (http_status == 204 || http_status == 205)
            && options.envelope != YM_RESPONSE_RESULT) {
            *out = result.release();
            return YM_OK;
        }
        const auto bytes
            = body.size ? std::string_view(body.data, body.size) : std::string_view { };
        if (!valid_utf8(bytes))
            return response_fail(YM_SCHEMA_ERROR, "Response is not UTF-8");
        sax parser(options);
        if (!nlohmann::json::sax_parse(bytes.begin(), bytes.end(), &parser) || !parser.has_root)
            return response_fail(parser.failure, parser.message);
        auto& root = parser.root;
        const auto* service_error = member(root, "error");
        if (http_failure != YM_OK || has_error(service_error)) {
            const auto code
                = http_failure == YM_OK ? static_cast<ym_status>(YM_SERVICE_ERROR) : http_failure;
            fail(error, code, "Service rejected the request");
            if (error) {
                error->http_status = http_status;
                if (const auto* value = text(service_error))
                    copy_text(error->server_code, *value);
                if (const auto* description
                    = text(alternative(root, "errorDescription", "error_description")))
                    copy_text(error->message, *description);
                copy_text(error->request_id, request_id(root));
            }
            return code;
        }
        result->request_id = request_id(root);
        if (options.envelope == YM_RESPONSE_ROOT)
            result->root = std::move(root);
        else {
            if (!std::holds_alternative<ym_value::object>(root.data))
                return response_fail(YM_SCHEMA_ERROR, "Response envelope is not an object");
            auto* payload = const_cast<ym_value*>(member(root, "result"));
            if (options.envelope == YM_RESPONSE_RESULT && !payload)
                return response_fail(YM_SCHEMA_ERROR, "Required result envelope is missing");
            if (payload
                && (options.envelope == YM_RESPONSE_RESULT
                    || !std::holds_alternative<std::monostate>(payload->data)))
                result->root = std::move(*payload);
            else
                result->root = std::move(root);
        }
        *out = result.release();
        return YM_OK;
    } catch (const std::bad_alloc&) {
        return fail(error, YM_OUT_OF_MEMORY, "Response allocation failed");
    } catch (...) {
        return fail(error, YM_SCHEMA_ERROR, "Response could not be decoded");
    }
}
