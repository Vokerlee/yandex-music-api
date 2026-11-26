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

#include <yandex_music/response.h>
#include <yandex_music/result.hpp>

namespace ym {

struct ResponseOptions {
    size_t max_bytes{8 * 1024 * 1024};
    size_t max_depth{64};
    size_t max_nodes{100000};
    size_t max_collection_items{100000};
    ym_response_envelope envelope{YM_RESPONSE_AUTO};
};

[[nodiscard]] inline Result<Document> parse_response(std::string_view body,
    uint32_t http_status = 200, ResponseOptions input = {}) noexcept
{
    ym_response_options options;
    ym_response_options_init(&options);
    options.max_bytes = input.max_bytes;
    options.max_depth = input.max_depth;
    options.max_nodes = input.max_nodes;
    options.max_collection_items = input.max_collection_items;
    options.envelope = input.envelope;

    ym_result* value = nullptr;
    ym_error failure;
    if (ym_response_parse(http_status, {body.data(), body.size()}, &options, &value, &failure) !=
        YM_OK)
        return std::unexpected(Error{failure});

    return Document{value};
}

} // namespace ym
