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
    using Array = std::vector<ym_value>;
    using Object = std::vector<std::pair<std::string, ym_value>>;
    using Storage =
        std::variant<std::monostate, bool, int64_t, uint64_t, double, std::string, Array, Object>;

    ym_value() = default;

    template<class T>
    explicit ym_value(T input)
        : data{std::move(input)}
    {
    }

    Storage data;
};

struct ym_result {
    ym_value root;
};
