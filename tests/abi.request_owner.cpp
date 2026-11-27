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

#include "core/client.hpp"
#include "core/engine.hpp"
#include "core/error.hpp"
#include "test_support.hpp"

#include <deque>
#include <map>

#include <yandex_music/yandex_music.hpp>
using namespace ym::detail;

struct FakeTransport final : Transport {
    std::map<uint64_t, const RequestInput*> active;
    std::deque<TransportResponse> ready;

    void start(uint64_t ticket, const RequestInput& input) override
    {
        active.emplace(ticket, &input);
    }

    void cancel(uint64_t ticket) noexcept override { active.erase(ticket); }

    std::optional<TransportResponse> poll(uint64_t) override
    {
        if (ready.empty())
            return {};
        auto item = std::move(ready.front());
        ready.pop_front();
        active.erase(item.ticket);
        return item;
    }

    void complete(uint64_t ticket, std::string text = "complete")
    {
        ready.push_back({ticket, YM_OK, 200, std::move(text)});
    }
};

ym_status decode_bytes(const TransportResponse& response, ym_result** out, ym_error* error) noexcept
{
    *out = nullptr;
    try {
        *out = new ym_result{ym_value(response.body)};
        return YM_OK;
    } catch (...) {
        return fail(error, YM_OUT_OF_MEMORY, "Test decoder allocation failed");
    }
}

int main()
{
    auto client = ym::Client::create();

    CHECK(client);

    auto test_adapter = std::make_shared<FakeTransport>();
    client->native_handle()->execution->adapter = test_adapter;
    ym_request* request = nullptr;

    CHECK(submit(client->native_handle(), {}, decode_bytes, nullptr, &request, nullptr) == YM_OK);
    CHECK(ym_request_get_status(request) == YM_PENDING);
    CHECK(ym_client_poll(client->native_handle(), 0, 8, nullptr) == YM_OK);

    test_adapter->complete(request->state->ticket);

    CHECK(ym_client_poll(client->native_handle(), 1, 8, nullptr) == YM_OK);

    ym_result* result = nullptr;

    CHECK(ym_request_take_result(request, &result, nullptr) == YM_OK);

    auto* owned = result;

    CHECK(ym_request_take_result(request, &result, nullptr) == YM_INVALID_STATE &&
        result == nullptr);

    ym_result_destroy(owned);
    ym_request_destroy(request);
    ym_request_destroy(nullptr);
}
