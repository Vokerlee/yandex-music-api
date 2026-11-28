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

#include "core/engine.hpp"
#include "core/error.hpp"
#include "test_support.hpp"

#include <deque>
#include <map>
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
    auto owner = std::make_shared<Engine>();
    auto adapter = std::make_shared<FakeTransport>();
    owner->adapter = adapter;
    owner->max_pending = 1;
    owner->max_response_bytes = 4;
    ym_request_options options;
    ym_request_options_init(&options);
    options.deadline_ms = 2;
    auto first = queue_state(owner, {}, decode_bytes, options);

    CHECK(first);
    CHECK(!queue_state(owner, {}, decode_bytes, options));
    CHECK(poll_engine(*owner, 2, 8, nullptr) == YM_OK &&
        (*first)->failure.code == YM_DEADLINE_EXCEEDED);
    CHECK(adapter->active.empty());

    options.deadline_ms = UINT64_MAX;
    auto next = queue_state(owner, {}, decode_bytes, options);

    CHECK(next);
    CHECK(poll_engine(*owner, 3, 1, nullptr) == YM_OK);

    adapter->complete((*next)->ticket, "oversized");

    CHECK(poll_engine(*owner, 4, 1, nullptr) == YM_OK &&
        (*next)->failure.code == YM_RESOURCE_LIMIT);
    CHECK(poll_engine(*owner, 3, 1, nullptr) == YM_INVALID_ARGUMENT);
    CHECK(poll_engine(*owner, 5, 0, nullptr) == YM_INVALID_ARGUMENT);
}
