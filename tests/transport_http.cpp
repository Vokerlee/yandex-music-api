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

#include "transport/curl_adapter.hpp"

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <thread>
#include <vector>

#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            std::fprintf(stderr, "Check failed at line %d: %s\n", __LINE__, #x);                   \
            std::abort();                                                                          \
        }                                                                                          \
    } while (false)

using namespace ym::detail;
using Clock = std::chrono::steady_clock;

TransportResponse receive(Transport& adapter)
{
    const auto deadline = Clock::now() + std::chrono::seconds(5);
    while (Clock::now() < deadline) {
        if (auto response = adapter.poll(0))
            return std::move(*response);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    CHECK(false);

    return {};
}

std::vector<RequestInput> echo_requests(const std::string& origin)
{
    std::vector<RequestInput> inputs;
    for (const auto* method : {"GET", "HEAD", "POST", "PUT", "PATCH", "DELETE"}) {
        inputs.push_back({
            .method = method,
            .url = origin + "/echo/a%2Fb?one=a+b&x=1&x=2",
            .headers = {{"X-Test", "yes"}, {"X-Empty", ""}}
        });
        if (inputs.back().method != "GET" && inputs.back().method != "HEAD")
            inputs.back().body = std::string("A\0B", 3);
    }
    return inputs;
}

int main(int argc, char** argv)
{
    CHECK(argc == 2);

    const std::string origin = argv[1];
    CurlPolicy local;
    local.allow_loopback_http = true;
    auto adapter = make_curl_transport(local);

    CHECK(adapter);

    RequestInput cancelled{.method = "GET", .url = origin + "/cancelled"};
    (*adapter)->start(777, cancelled);
    (*adapter)->cancel(777);
    auto inputs = echo_requests(origin);
    for (size_t i = 0; i < inputs.size(); ++i)
        (*adapter)->start(i + 1, inputs[i]);
    std::vector<bool> seen(inputs.size());
    for (size_t i = 0; i < inputs.size(); ++i) {
        auto response = receive(**adapter);

        CHECK(response.ticket >= 1 && response.ticket <= inputs.size());

        const auto index = static_cast<size_t>(response.ticket - 1);

        CHECK(!seen[index]);

        seen[index] = true;

        CHECK(response.status == YM_OK && response.http_status == 200);
        CHECK(response.request_id == "fixture" && response.retry_after_ms == 2000);

        const auto& input = inputs[index];
        const auto expected = input.method == "HEAD"
            ? std::string{}
            : input.method + "|/echo/a%2Fb?one=a+b&x=1&x=2|yes|" + input.body;

        CHECK(response.body == expected);
    }

    CHECK(!(*adapter)->poll(0));

    local.max_body = 16;
    auto limited = make_curl_transport(local);

    CHECK(limited);

    RequestInput large{.method = "GET", .url = origin + "/large"};
    RequestInput exact{.method = "GET", .url = origin + "/exact"};
    (*limited)->start(1, large);
    (*limited)->start(2, exact);
    for (int i = 0; i < 2; ++i) {
        const auto response = receive(**limited);
        if (response.ticket == 1)
            CHECK(response.status == YM_RESOURCE_LIMIT && response.body.size() <= 16);
        else
            CHECK(response.ticket == 2 && response.status == YM_OK &&
                response.body == std::string(16, 'x'));
    }
}
