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

#include "api_support.hpp"

int main()
{
    ApiFixture fixture;
    const ym_string_view path[]{
        {"account", 7},
        {"status",  6}
    };
    ym_api_call call{};
    call.method = {"GET", 3};
    call.path = path;
    call.path_count = 2;
    auto result = fixture.client.wait(fixture.client.request(call));
    CHECK(result && result->root().member("ok")->boolean() == true);

    auto request = fixture.client.request(call);
    CHECK(request);
    auto timeout = fixture.client.wait(*request, 0);
    CHECK(!timeout && timeout.error().code() == YM_DEADLINE_EXCEEDED);
    CHECK(request->status() == YM_DEADLINE_EXCEEDED);

    request = fixture.client.request(call);
    CHECK(request);
    auto other = ym::Client::create();
    CHECK(other);
    auto rejected = other->wait(*request);
    CHECK(!rejected && rejected.error().code() == YM_INVALID_ARGUMENT);
    CHECK(fixture.client.wait(*request));
    CHECK(fixture.client.wait(*request).error().code() == YM_INVALID_STATE);
}
