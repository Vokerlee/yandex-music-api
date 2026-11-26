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

#include <yandex_music/types.h>

namespace ym::detail {

struct ResponseSink {
    explicit ResponseSink(size_t limit)
        : max_body{limit}
    {
    }

    size_t max_body;
    size_t max_headers{64 * 1024};
    size_t header_bytes{0};
    std::string request_id;
    int64_t retry_after_ms{-1};
    std::string body;
    ym_status failure{YM_OK};
};

size_t collect_header(char*, size_t, size_t, void*) noexcept;
size_t collect_body(char*, size_t, size_t, void*) noexcept;

} // namespace ym::detail
