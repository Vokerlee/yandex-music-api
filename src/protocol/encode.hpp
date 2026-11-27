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

#pragma once

#include "core/engine.hpp"
#include "fields.hpp"

namespace ym::detail::wire {

enum class HttpMethod {
    get,
    post,
    put,
    remove,
    patch,
    head,
};

struct RequestSpec {
    std::string origin{};
    HttpMethod verb = HttpMethod::get;
    std::vector<std::string> path{};
    std::vector<RequestField> query{};
    bool mutating = false;
};

std::expected<RequestInput, ym_error> encode(const RequestSpec&,
    size_t max_bytes = 1024 * 1024) noexcept;

} // namespace ym::detail::wire
