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
#include "core/error.hpp"
#include "test_support.hpp"
#include <deque>
#include <map>
#include <stdexcept>
#include <yandex_music/yandex_music.hpp>

using namespace ym::detail;
struct fake_transport final : transport {
    std::map<uint64_t, const request_input*> active;
    std::deque<transport_response> ready;
    void start(uint64_t ticket, const request_input& input) override
    {
        active.emplace(ticket, &input);
    }
    void cancel(uint64_t ticket) noexcept override { active.erase(ticket); }
    std::optional<transport_response> poll(uint64_t) override
    {
        if (ready.empty())
            return { };
        auto value = std::move(ready.front());
        ready.pop_front();
        active.erase(value.ticket);
        return value;
    }
    void complete(uint64_t ticket, std::string body = "complete")
    {
        ready.push_back({ ticket, YM_OK, 200, std::move(body) });
    }
};
ym_status decode(const transport_response& response, ym_result** out, ym_error* error) noexcept
{
    *out = nullptr;
    try {
        *out = new ym_result { ym_value(response.body) };
        return YM_OK;
    } catch (...) {
        return fail(error, YM_OUT_OF_MEMORY, "Test decoder allocation failed");
    }
}
struct callback_context {
    int calls = 0;
    ym_client* client = nullptr;
    bool reentrancy_ok = true;
    bool destroy_request = false;
    bool destroy_client = false;
    bool submit_child = false;
    ym_request* child = nullptr;
};
void YM_CALL completed(ym_request* request, ym_status, void* userdata)
{
    auto& context = *static_cast<callback_context*>(userdata);
    ++context.calls;
    context.reentrancy_ok = context.reentrancy_ok
        && ym_client_poll(context.client, 0, 8, nullptr) == YM_REENTRANT_CALL
        && ym_client_close(context.client, nullptr) == YM_REENTRANT_CALL;
    if (context.submit_child) {
        context.reentrancy_ok = context.reentrancy_ok
            && submit(context.client, { }, decode, nullptr, &context.child, nullptr) == YM_OK;
    }
    if (context.destroy_request)
        ym_request_destroy(request);
    if (context.destroy_client)
        ym_client_destroy(context.client);
}
void YM_CALL throws(ym_request*, ym_status, void*)
{
    throw std::runtime_error("callback");
}

int main()
{
    auto client = ym::client::create({ .max_pending_requests = 2 });
    CHECK(client);
    auto transport = std::make_shared<fake_transport>();
    client->native_handle()->execution->adapter = transport;
    callback_context context { .client = client->native_handle() };
    ym_request_options options;
    ym_request_options_init(&options);
    options.on_complete = completed;
    options.userdata = &context;
    request_input input {
        .method = "GET", .url = "https://example.invalid/owned", .body = "owned"
    };
    ym_request* raw = nullptr;
    CHECK(submit(client->native_handle(), input, decode, &options, &raw, nullptr) == YM_OK);
    input.body = "changed";
    ym::request request(raw);
    CHECK(request.state() == YM_REQUEST_QUEUED && request.status() == YM_PENDING);
    CHECK(!request.take_result() && request.take_result().error().code() == YM_PENDING);
    CHECK(client->poll(0, 1));
    const auto ticket = raw->state->ticket;
    CHECK(request.state() == YM_REQUEST_RUNNING && transport->active.at(ticket)->body == "owned");
    transport->complete(ticket);
    CHECK(client->poll(1, 1));
    CHECK(context.calls == 0 && request.status() == YM_OK);
    CHECK(client->poll(2, 1));
    CHECK(context.calls == 1 && context.reentrancy_ok);
    CHECK(client->poll(3) && context.calls == 1);
    CHECK(!client->poll(2));
    auto document = request.take_result();
    CHECK(document && *document->root().string() == "complete");
    CHECK(!request.take_result() && request.take_result().error().code() == YM_INVALID_STATE);
    CHECK(request.cancel() && request.status() == YM_OK);
    CHECK(submit(client->native_handle(), { }, decode, &options, &raw, nullptr) == YM_OK);
    const auto stale_ticket = raw->state->ticket;
    CHECK(client->poll(4));
    CHECK(ym_request_cancel(raw, nullptr) == YM_OK && transport->active.empty());
    transport->complete(stale_ticket);
    ym_request* newer = nullptr;
    CHECK(submit(client->native_handle(), { }, decode, &options, &newer, nullptr) == YM_OK);
    CHECK(newer->state->ticket != stale_ticket);
    CHECK(client->poll(5) && ym_request_get_status(newer) == YM_PENDING);
    CHECK(ym_request_get_status(raw) == YM_CANCELLED && context.calls == 2);
    transport->complete(newer->state->ticket);
    CHECK(client->poll(6) && context.calls == 3);
    ym_request_destroy(raw);
    ym_request_destroy(newer);
    options.deadline_ms = 7;
    CHECK(submit(client->native_handle(), { }, decode, &options, &raw, nullptr) == YM_OK);
    CHECK(client->poll(7) && ym_request_get_status(raw) == YM_DEADLINE_EXCEEDED);
    CHECK(transport->active.empty());
    ym_request_destroy(raw);
    options.deadline_ms = 10;
    CHECK(submit(client->native_handle(), { .mutating = true }, decode, &options, &raw, nullptr)
        == YM_OK);
    CHECK(client->poll(8));
    CHECK(client->poll(10) && ym_request_get_status(raw) == YM_OUTCOME_UNKNOWN);
    ym_request_destroy(raw);
    ym_request_options_init(&options);
    options.on_complete = completed;
    options.userdata = &context;
    context.destroy_request = true;
    context.submit_child = true;
    CHECK(submit(client->native_handle(), { }, decode, &options, &raw, nullptr) == YM_OK);
    CHECK(client->poll(11));
    transport->complete(raw->state->ticket);
    CHECK(client->poll(12) && context.child && context.reentrancy_ok);
    CHECK(ym_request_get_state(context.child) == YM_REQUEST_QUEUED);
    CHECK(client->poll(13));
    CHECK(client->close() && transport->active.empty());
    CHECK(ym_request_get_status(context.child) == YM_CANCELLED);
    ym_request_destroy(context.child);
    CHECK(!client->poll(14) && client->poll(14).error().code() == YM_CLOSED);
    CHECK(*document->root().string() == "complete");
    ym_client* other = nullptr;
    CHECK(ym_client_create(nullptr, &other, nullptr) == YM_OK);
    auto other_transport = std::make_shared<fake_transport>();
    other->execution->adapter = other_transport;
    callback_context destroying { .client = other, .destroy_client = true };
    options.on_complete = completed;
    options.userdata = &destroying;
    CHECK(submit(other, { }, decode, &options, &raw, nullptr) == YM_OK);
    ym_request* survivor = nullptr;
    CHECK(submit(other, { }, decode, nullptr, &survivor, nullptr) == YM_OK);
    CHECK(ym_client_poll(other, 0, 64, nullptr) == YM_OK);
    other_transport->complete(raw->state->ticket);
    CHECK(ym_client_poll(other, 1, 64, nullptr) == YM_OK); // destroys other after callback returns
    CHECK(destroying.calls == 1 && other_transport->active.empty());
    CHECK(ym_request_get_status(survivor) == YM_CANCELLED);
    ym_request_destroy(raw);
    ym_request_destroy(survivor);
    CHECK(ym_client_create(nullptr, &other, nullptr) == YM_OK);
    other->execution->adapter = other_transport;
    options.on_complete = throws;
    CHECK(submit(other, { }, decode, &options, &raw, nullptr) == YM_OK);
    CHECK(ym_client_poll(other, 0, 64, nullptr) == YM_OK);
    other_transport->complete(raw->state->ticket);
    CHECK(ym_client_poll(other, 1, 64, nullptr) == YM_INTERNAL_ERROR);
    CHECK(ym_client_poll(other, 2, 64, nullptr) == YM_OK);
    ym_request_destroy(raw);
    ym_client_destroy(other);
    auto limited = ym::client::create({ .max_response_bytes = 4, .max_pending_requests = 1 });
    CHECK(limited);
    limited->native_handle()->execution->adapter = other_transport;
    ym_request_options_init(&options);
    CHECK(submit(limited->native_handle(), { }, decode, &options, &raw, nullptr) == YM_OK);
    ym_request* overflow = reinterpret_cast<ym_request*>(1);
    CHECK(submit(limited->native_handle(), { }, decode, nullptr, &overflow, nullptr)
            == YM_RESOURCE_LIMIT
        && overflow == nullptr);
    CHECK(limited->poll(0));
    other_transport->complete(raw->state->ticket, "oversized");
    CHECK(limited->poll(1) && ym_request_get_status(raw) == YM_RESOURCE_LIMIT);
    ym_request_destroy(raw);
    options.version = 9;
    CHECK(submit(limited->native_handle(), { }, decode, &options, &raw, nullptr) == YM_UNSUPPORTED
        && raw == nullptr);
    CHECK(submit(limited->native_handle(), { }, decode, nullptr, &raw, nullptr) == YM_OK);
    CHECK(limited->poll(UINT64_MAX)
        && ym_request_get_status(raw) == YM_PENDING); // no deadline sentinel
    ym_request_destroy(raw);
    CHECK(ym_client_poll(nullptr, 0, 1, nullptr) == YM_INVALID_ARGUMENT);
    CHECK(ym_request_cancel(nullptr, nullptr) == YM_INVALID_ARGUMENT);
}
