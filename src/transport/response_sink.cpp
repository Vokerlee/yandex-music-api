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

#include "response_sink.hpp"

#include <limits>
#include <new>

namespace ym::detail {

size_t collect_body(char* data, size_t size, size_t count, void* userdata) noexcept
{
    auto* sink = static_cast<ResponseSink*>(userdata);
    if (!sink || sink->failure != YM_OK)
        return 0;
    if (size && count > std::numeric_limits<size_t>::max() / size) {
        sink->failure = YM_RESOURCE_LIMIT;
        return 0;
    }
    const size_t bytes = size * count;
    if (!bytes)
        return 0;
    if (!data) {
        sink->failure = YM_INTERNAL_ERROR;
        return 0;
    }
    const auto used = sink->body.size();
    if (used > sink->max_body || bytes > sink->max_body - used) {
        sink->failure = YM_RESOURCE_LIMIT;
        return 0;
    }

    try {
        sink->body.append(data, bytes);
        return bytes;
    } catch (const std::bad_alloc&) {
        sink->failure = YM_OUT_OF_MEMORY;
    } catch (...) {
        sink->failure = YM_INTERNAL_ERROR;
    }
    return 0;
}

} // namespace ym::detail
