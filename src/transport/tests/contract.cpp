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

#include "core/transport.hpp"
#include "test_support.hpp"

#include <map>
using namespace ym::detail;

struct Adapter : Transport {
    const RequestInput* borrowed = nullptr;

    void start(uint64_t, const RequestInput& input) override { borrowed = &input; }

    void cancel(uint64_t) noexcept override { borrowed = nullptr; }

    std::optional<TransportResponse> poll(uint64_t) override { return {}; }
};

int main()
{
    RequestInput original;
    original.body = "owned";
    auto copied = original;
    original.body = "changed";
    Adapter Transport;
    Transport.start(1, copied);

    CHECK(Transport.borrowed->body == "owned");

    Transport.cancel(1);

    CHECK(Transport.borrowed == nullptr);

    ym_request_options options;
    ym_request_options_init(&options);

    CHECK(options.deadline_ms == UINT64_MAX);
}
