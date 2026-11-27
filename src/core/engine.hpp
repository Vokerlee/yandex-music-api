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
#include "transport.hpp"

#include <expected>

#include <yandex_music/request.h>

namespace ym::detail {

using ResponseDecoder = ym_status (*)(const TransportResponse&, ym_result**, ym_error*) noexcept;
struct Engine;

struct RequestState {
    std::weak_ptr<Engine> owner;
    RequestInput input;
    ResponseDecoder decode = nullptr;
    ym_request_options options{};
    ym_request_state phase = YM_REQUEST_QUEUED;
    uint64_t ticket = 0;
    ym_error failure{};
    std::unique_ptr<ym_result> result;
    ym_request* handle = nullptr;
    bool delivered = false;
    bool suppressed = false;
};

struct Engine {
    std::shared_ptr<Transport> adapter;
    std::vector<std::shared_ptr<RequestState>> requests;
    uint64_t next_ticket = 1, last_time = 0;
    size_t max_pending = 64, max_response_bytes = 8 * 1024 * 1024;
    bool closed = false, driving = false, has_time = false, stopping = false;
    ~Engine();
};

ym_status submit(ym_client*, RequestInput, ResponseDecoder, const ym_request_options*, ym_request**,
    ym_error*) noexcept;
bool pending(const RequestState&) noexcept;
void cancel_state(RequestState&, ym_status reason = YM_CANCELLED,
    Engine* context = nullptr) noexcept;
void close_engine(Engine&) noexcept;
std::expected<std::shared_ptr<RequestState>, ym_error> queue_state(const std::shared_ptr<Engine>&,
    RequestInput, ResponseDecoder, const ym_request_options&) noexcept;
ym_status poll_engine(Engine&, uint64_t, size_t, ym_error*) noexcept;

} // namespace ym::detail

struct ym_request {
    std::shared_ptr<ym::detail::RequestState> state;
    bool in_callback = false, destroy_requested = false;
};
