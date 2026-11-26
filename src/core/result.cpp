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

#include "error.hpp"
#include "result.hpp"

#include <new>

namespace {

template<class T>
const T* value_data(const ym_value* value) noexcept
{
    return value ? std::get_if<T>(&value->data) : nullptr;
}

template<class T>
ym_status scalar(const ym_value* value, T* out, ym_error* error) noexcept
{
    ym::detail::clear(error);
    if (!out)
        return ym::detail::fail(error, YM_INVALID_ARGUMENT, "Value output is required");

    *out = {};
    if (const auto* item = value_data<T>(value)) {
        *out = *item;
        return YM_OK;
    }

    return ym::detail::fail(error, YM_SCHEMA_ERROR, "Value has a different type");
}

} // namespace

void YM_CALL ym_result_destroy(ym_result* result) YM_NOEXCEPT
{
    delete result;
}

ym_status YM_CALL ym_result_clone(const ym_result* input, ym_result** out,
    ym_error* error) YM_NOEXCEPT
{
    using namespace ym::detail;
    clear(error);
    if (!out)
        return fail(error, YM_INVALID_ARGUMENT, "Result output is required");
    *out = nullptr;
    if (!input)
        return fail(error, YM_INVALID_ARGUMENT, "Result is required");
    try {
        *out = new ym_result(*input);
        return YM_OK;
    } catch (const std::bad_alloc&) {
        return fail(error, YM_OUT_OF_MEMORY, "Result copy allocation failed");
    } catch (...) {
        return fail(error, YM_INTERNAL_ERROR, "Result copy failed");
    }
}

const ym_value* YM_CALL ym_result_root(const ym_result* result) YM_NOEXCEPT
{
    return result ? &result->root : nullptr;
}

ym_value_type YM_CALL ym_value_typeof(const ym_value* value) YM_NOEXCEPT
{
    return value ? static_cast<ym_value_type>(value->data.index() + 1)
                 : static_cast<ym_value_type>(YM_VALUE_MISSING);
}

ym_status YM_CALL ym_value_string(const ym_value* value, ym_string_view* out,
    ym_error* error) YM_NOEXCEPT
{
    using namespace ym::detail;
    clear(error);
    if (!out)
        return fail(error, YM_INVALID_ARGUMENT, "String output is required");

    *out = {};
    if (const auto* text = value_data<std::string>(value)) {
        *out = {text->data(), text->size()};
        return YM_OK;
    }

    return fail(error, YM_SCHEMA_ERROR, "Value is not a string");
}

ym_status YM_CALL ym_value_int64(const ym_value* value, int64_t* out, ym_error* error) YM_NOEXCEPT
{
    return scalar(value, out, error);
}

ym_status YM_CALL ym_value_uint64(const ym_value* value, uint64_t* out, ym_error* error) YM_NOEXCEPT
{
    return scalar(value, out, error);
}

ym_status YM_CALL ym_value_double(const ym_value* value, double* out, ym_error* error) YM_NOEXCEPT
{
    return scalar(value, out, error);
}

ym_status YM_CALL ym_value_bool(const ym_value* value, uint32_t* out, ym_error* error) YM_NOEXCEPT
{
    if (!out)
        return ym::detail::fail(error, YM_INVALID_ARGUMENT, "Boolean output is required");
    *out = 0;
    bool result = false;
    const auto status = scalar(value, &result, error);
    if (status == YM_OK)
        *out = result ? 1 : 0;
    return status;
}
