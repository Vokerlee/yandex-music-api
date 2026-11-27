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

#include "core/result.hpp"

#include <expected>
#include <string_view>

namespace ym::detail {

struct JsonLimits {
    size_t max_bytes = 8 * 1024 * 1024;
    size_t max_depth = 64;
    size_t max_nodes = 100000;
    size_t max_collection_items = 100000;
};

std::expected<ym_value, ym_error> read_json(std::string_view, JsonLimits limits = {}) noexcept;

} // namespace ym::detail
