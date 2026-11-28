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
#include <cstdlib>
#include <thread>

#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x))                                                                                  \
            std::abort();                                                                          \
    } while (false)

using namespace ym::detail;

int main()
{
    CurlPolicy local;
    local.allow_loopback_http = true;
    auto adapter = make_curl_transport(local);

    CHECK(adapter);

    RequestInput input{.method = "GET", .url = "http://127.0.0.1:1/refused"};
    (*adapter)->start(7, input);
    (*adapter)->cancel(7);
    (*adapter)->cancel(7);

    CHECK(!(*adapter)->poll(0));

    (*adapter)->start(9, input);
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(3);
    std::optional<TransportResponse> response;
    while (!(response = (*adapter)->poll(0)) && std::chrono::steady_clock::now() < deadline)
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

    CHECK(response && response->ticket == 9 && response->status == YM_NETWORK_ERROR);
    CHECK(response->http_status == 0 && !(*adapter)->poll(0));
}
