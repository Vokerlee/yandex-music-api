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
