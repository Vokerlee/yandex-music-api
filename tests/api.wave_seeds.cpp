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
    std::string mood{"settingMoodEnergy:active"};
    auto request =
        fixture.client.wave_start_with("user:onyourwave", "settingDiversity:discover", mood);
    mood = "changed";
    CHECK(fixture.client.wait(std::move(request)));
    CHECK(
        fixture.transport->input.body ==
        R"({"seeds":["user:onyourwave","settingDiversity:discover","settingMoodEnergy:active"],"includeTracksInResponse":true})"
    );

    const ym_string_view seeds[]{
        {"artist:42",               9 },
        {"settingLanguage:russian", 23}
    };
    CHECK(fixture.client.wait(fixture.client.wave_start_seeds(seeds)));
    CHECK(fixture.transport->input.body.find("artist:42") != std::string::npos);
    CHECK(!fixture.client.wave_start_seeds({}));
}
