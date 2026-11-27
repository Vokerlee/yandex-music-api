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

#include "protocol/fields.hpp"
#include "test_support.hpp"
using namespace ym::detail;
using namespace ym::detail::wire;

int main()
{
    std::vector<RequestField> fields{
        {"omit",  std::nullopt              },
        {"id",    ym_value(std::string("1"))},
        {"id",    ym_value(std::string("2"))},
        {"empty", ym_value(std::string{})   },
        {"flag",  ym_value(false)           }
    };

    CHECK(*encode_fields(fields, false, 256) == "id=1&id=2&empty=&flag=false");

    fields = {
        {"integer", ym_value(uint64_t(18446744073709551615ULL))}
    };

    CHECK(*encode_fields(fields, true, 256) == "integer=18446744073709551615");

    fields.push_back({"null", ym_value()});

    CHECK(!encode_fields(fields, false, 256));
}
