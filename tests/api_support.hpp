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

#include "core/client.hpp"
#include "test_support.hpp"

#include <optional>

#include <yandex_music/yandex_music.hpp>

struct ApiTransport final : ym::detail::Transport {
    ym::detail::RequestInput input;
    std::string body{R"({"result":{"ok":true}})"};
    uint64_t ticket{0};
    uint32_t http_status{200};

    void start(uint64_t id, const ym::detail::RequestInput& value) override
    {
        input = value;
        ticket = id;
    }

    void cancel(uint64_t) noexcept override { ticket = 0; }

    std::optional<ym::detail::TransportResponse> poll(uint64_t) override
    {
        if (!ticket)
            return {};
        ym::detail::TransportResponse response;
        response.ticket = std::exchange(ticket, 0);
        response.body = body;
        response.http_status = http_status;
        return response;
    }
};

struct ApiFixture {
    ym::Client client{std::move(*ym::Client::create({.token = "test-token"}))};
    std::shared_ptr<ApiTransport> transport{std::make_shared<ApiTransport>()};
    uint64_t now{0};

    ApiFixture() { client.native_handle()->execution->adapter = transport; }

    ym::Result<ym::Document> complete(ym::Result<ym::Request> request)
    {
        if (!request)
            return std::unexpected(request.error());
        auto advanced = client.poll(++now);
        if (!advanced)
            return std::unexpected(advanced.error());
        return request->take_result();
    }
};
