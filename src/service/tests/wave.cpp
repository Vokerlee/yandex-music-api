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
#include "protocol/json_read.hpp"

int main()
{
    ApiFixture fixture;
    fixture.transport->body =
        R"({"result":{"radioSessionId":"session-1","batchId":"batch-1","sequence":[{"track":{"id":"42"}}]}})";
    auto wave = fixture.client.wait(fixture.client.wave_start());
    CHECK(wave && wave->root().member("radioSessionId")->string() == "session-1");
    CHECK(fixture.transport->input.url.ends_with("/rotor/session/new"));
    CHECK(fixture.transport->input.body ==
        R"({"seeds":["user:onyourwave"],"includeTracksInResponse":true})");

    CHECK(fixture.client.wait(fixture.client.wave_next("session-1", "42:8,43")));
    CHECK(fixture.transport->input.url.ends_with("/rotor/session/session-1/tracks"));
    CHECK(fixture.transport->input.body == R"({"queue":["42:8","43"]})");
    CHECK(fixture.client.wait(fixture.client.wave_feedback("session-1", "skip", "42:8", 12.5,
        "batch-1")));
    auto feedback = ym::detail::read_json(fixture.transport->input.body);
    CHECK(feedback);
    const ym_value* event = nullptr;
    CHECK(ym_value_member(&*feedback, {"event", 5}, &event, nullptr) == YM_OK);
    const ym_value* played = nullptr;
    CHECK(ym_value_member(event, {"totalPlayedSeconds", 18}, &played, nullptr) == YM_OK);
    double seconds = 0;
    CHECK(ym_value_double(played, &seconds, nullptr) == YM_OK && seconds == 12.5);
    CHECK(fixture.transport->input.mutating);
    CHECK(!fixture.client.wave_feedback("session-1", "trackStarted"));
}
