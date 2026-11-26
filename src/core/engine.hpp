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
#include "result.hpp"
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <yandex_music/request.h>

namespace ym::detail
{
struct request_input {
    std::string method { };
    std::string url { };
    std::vector<std::pair<std::string, std::string>> headers { };
    std::string body { };
    bool mutating = false;
};
struct transport_response {
    uint64_t ticket = 0;
    ym_status status = YM_OK;
    uint32_t http_status = 200;
    std::string body { };
    std::string request_id { };
    int64_t retry_after_ms = -1;
};
/** start is atomic on throw; cancel synchronously detaches all borrowed input access.
 * No callback is invoked by an adapter. Completion tickets must be preserved exactly. */
struct transport {
    virtual ~transport() = default;
    virtual void start(uint64_t ticket, const request_input& input) = 0;
    virtual void cancel(uint64_t ticket) noexcept = 0;
    virtual std::optional<transport_response> poll(uint64_t now_ms) = 0;
};
using decoder_fn = ym_status (*)(const transport_response&, ym_result**, ym_error*) noexcept;
struct engine;
struct request_state {
    std::weak_ptr<engine> owner;
    request_input input;
    decoder_fn decode = nullptr;
    ym_request_options options { };
    ym_request_state phase = YM_REQUEST_QUEUED;
    uint64_t ticket = 0;
    size_t max_response_bytes = 0;
    ym_error failure { };
    std::unique_ptr<ym_result> result;
    ym_request* handle = nullptr;
    bool delivered = false;
    bool suppressed = false;
};
struct engine {
    std::shared_ptr<transport> adapter;
    std::vector<std::weak_ptr<request_state>> requests;
    uint64_t next_ticket = 1;
    uint64_t last_time = 0;
    bool has_time = false;
    bool driving = false;
    bool closed = false;
    bool stopping = false;
};
/** Private endpoint seam; real service methods supply owned input and an audited decoder. */
ym_status submit(ym_client*, request_input, decoder_fn, const ym_request_options*, ym_request**,
    ym_error*) noexcept;
} // namespace ym::detail

struct ym_request {
    std::shared_ptr<ym::detail::request_state> state;
    bool in_callback = false;
    bool destroy_requested = false;
};
