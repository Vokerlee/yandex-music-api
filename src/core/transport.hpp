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

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <yandex_music/c/request.h>

namespace ym::detail {

struct RequestInput {
    std::string method{};
    std::string url{};
    std::vector<std::pair<std::string, std::string>> headers{};
    std::string body{};
    bool mutating = false;
};

struct TransportResponse {
    uint64_t ticket = 0;
    ym_status status = YM_OK;
    uint32_t http_status = 200;
    std::string body{};
    std::string request_id{};
    int64_t retry_after_ms = -1;
};

/** start is atomic on throw; cancel synchronously detaches all borrowed input access.
 * No callback is invoked by an adapter. Completion tickets must be preserved exactly. */
struct Transport {
    virtual ~Transport() = default;
    virtual void start(uint64_t ticket, const RequestInput& input) = 0;
    virtual void cancel(uint64_t ticket) noexcept = 0;
    virtual std::optional<TransportResponse> poll(uint64_t now_ms) = 0;
};

} // namespace ym::detail
