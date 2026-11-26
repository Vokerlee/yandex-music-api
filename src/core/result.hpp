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
#include <string>
#include <utility>
#include <variant>
#include <vector>
#include <yandex_music/result.h>

struct ym_value {
    using array = std::vector<ym_value>;
    using object = std::vector<std::pair<std::string, ym_value>>;
    using storage
        = std::variant<std::monostate, bool, int64_t, uint64_t, double, std::string, array, object>;
    storage data;
    ym_value() = default;
    template <class T>
    explicit ym_value(T input)
        : data(std::move(input))
    {
    }
};
struct ym_result {
    ym_value root;
    uint32_t http_status = 0;
    std::string request_id { };
};
