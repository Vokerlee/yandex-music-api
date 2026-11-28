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
    ym_request_options options;
    ym_request_options_init(&options);
    RequestInput input;
    input.body = "owned";
    auto queued = queue_state(owner, input, decode_bytes, options);

    CHECK(queued);

    input.body = "changed";

    CHECK(poll_engine(*owner, 0, 1, nullptr) == YM_OK && (*queued)->phase == YM_REQUEST_RUNNING);
    CHECK(adapter->active.at((*queued)->ticket)->body == "owned");

    adapter->complete((*queued)->ticket);

    CHECK(poll_engine(*owner, 1, 1, nullptr) == YM_OK && (*queued)->phase == YM_REQUEST_SUCCEEDED);
    CHECK(std::get<std::string>((*queued)->result->root.data) == "complete");

    auto pending = queue_state(owner, {}, decode_bytes, options);

    CHECK(pending);
    CHECK(poll_engine(*owner, 2, 8, nullptr) == YM_OK);

    owner.reset();

    CHECK(adapter->active.empty() && (*pending)->phase == YM_REQUEST_CANCELLED);
}
