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

#include <yandex_music/cpp/yandex_music.hpp>
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

    auto adapter = std::make_shared<FakeTransport>();
    client->native_handle()->execution->adapter = adapter;
    ym_request* first = nullptr;

    CHECK(submit(client->native_handle(), {}, decode_bytes, nullptr, &first, nullptr) == YM_OK);
    CHECK(ym_client_poll(client->native_handle(), 0, 8, nullptr) == YM_OK);

    auto stale = first->state->ticket;

    CHECK(ym_request_cancel(first, nullptr) == YM_OK && adapter->active.empty());

    adapter->complete(stale);
    ym_request* next = nullptr;

    CHECK(submit(client->native_handle(), {}, decode_bytes, nullptr, &next, nullptr) == YM_OK);
    CHECK(next->state->ticket != stale &&
        ym_client_poll(client->native_handle(), 1, 8, nullptr) == YM_OK);
    CHECK(ym_request_get_status(next) == YM_PENDING &&
        ym_request_get_status(first) == YM_CANCELLED);

    ym_request_destroy(first);
    ym_request_destroy(next);
    RequestInput write;
    write.mutating = true;

    CHECK(submit(client->native_handle(), write, decode_bytes, nullptr, &first, nullptr) == YM_OK);
    CHECK(ym_client_poll(client->native_handle(), 2, 8, nullptr) == YM_OK);
    CHECK(ym_request_cancel(first, nullptr) == YM_OK &&
        ym_request_get_status(first) == YM_OUTCOME_UNKNOWN);

    ym_request_destroy(first);
}
