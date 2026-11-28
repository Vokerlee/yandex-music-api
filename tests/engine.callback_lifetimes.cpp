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
#include <stdexcept>

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

struct Context {
    ym_client* client;
    int calls = 0;
};

void YM_CALL destroy_client(ym_request*, ym_status, void* data)
{
    auto& c = *static_cast<Context*>(data);
    ++c.calls;
    ym_client_destroy(c.client);
}

void YM_CALL throws(ym_request*, ym_status, void*)
{
    throw std::runtime_error("callback");
}

int main()
{
    ym_client* client = nullptr;

    CHECK(ym_client_create(nullptr, &client, nullptr) == YM_OK);

    auto adapter = std::make_shared<FakeTransport>();
    client->execution->adapter = adapter;
    Context c{client};
    ym_request_options options;
    ym_request_options_init(&options);
    options.on_complete = destroy_client;
    options.userdata = &c;
    ym_request *first = nullptr, *other = nullptr;

    CHECK(submit(client, {}, decode_bytes, &options, &first, nullptr) == YM_OK);
    CHECK(submit(client, {}, decode_bytes, nullptr, &other, nullptr) == YM_OK);
    CHECK(ym_client_poll(client, 0, 8, nullptr) == YM_OK);

    adapter->complete(first->state->ticket);

    CHECK(ym_client_poll(client, 1, 8, nullptr) == YM_OK && c.calls == 1 &&
        adapter->active.empty());
    CHECK(ym_request_get_status(other) == YM_CANCELLED);

    ym_request_destroy(first);
    ym_request_destroy(other);

    CHECK(ym_client_create(nullptr, &client, nullptr) == YM_OK);

    client->execution->adapter = adapter;
    options.on_complete = throws;

    CHECK(submit(client, {}, decode_bytes, &options, &first, nullptr) == YM_OK);
    CHECK(ym_client_poll(client, 0, 8, nullptr) == YM_OK);

    adapter->complete(first->state->ticket);

    CHECK(ym_client_poll(client, 1, 8, nullptr) == YM_INTERNAL_ERROR);
    CHECK(ym_client_poll(client, 2, 8, nullptr) == YM_OK);

    ym_request_destroy(first);
    ym_client_destroy(client);
}
