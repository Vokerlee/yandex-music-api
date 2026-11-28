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

#include "protocol/encode.hpp"
#include "test_support.hpp"

#include <limits>
using namespace ym::detail;
using namespace ym::detail::wire;

int main()
{
    RequestSpec spec;
    spec.origin = "https://example.invalid";
    spec.encoding = BodyKind::json;
    spec.body = {
        {"omit",     std::nullopt                                                     },
        {"null",     ym_value()                                                       },
        {"integer",  ym_value(uint64_t(18446744073709551615ULL))                      },
        {"wire-key", ym_value(ym_value::Array{ym_value(false), ym_value(int64_t(-1))})}
    };

    CHECK(encode(spec)->body ==
        R"({"null":null,"integer":18446744073709551615,"wire-key":[false,-1]})");

    spec.body.push_back({"integer", ym_value(int64_t(2))});

    CHECK(!encode(spec));

    spec.body.pop_back();
    spec.body.push_back({"bad", ym_value(std::numeric_limits<double>::infinity())});

    CHECK(!encode(spec));
}
