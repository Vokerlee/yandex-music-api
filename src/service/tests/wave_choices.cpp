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
    fixture.transport->body =
        R"({"result":{"settingRestrictions":{"moodEnergy":{"possibleValues":[{"value":"active","name":"Energetic","serializedSeed":"settingMoodEnergy:active"}]}},"blocks":[]}})";
    auto settings = fixture.client.wait(fixture.client.wave_settings());
    CHECK(settings);
    auto choice = settings->root()["settingRestrictions"]["moodEnergy"]["possibleValues"][0];
    CHECK(choice["serializedSeed"].string() == "settingMoodEnergy:active");
    CHECK(fixture.transport->input.url.ends_with("/rotor/wave/settings?seeds=user%3Aonyourwave"));

    fixture.transport->body =
        R"({"result":[{"station":{"id":{"type":"genre","tag":"hip-hop"},"name":"Rap"}}]})";
    auto stations = fixture.client.wait(fixture.client.wave_stations());
    CHECK(stations && stations->root()[0]["station"]["id"]["tag"].string() == "hip-hop");
    CHECK(fixture.transport->input.url.ends_with("/rotor/stations/list?language=ru"));
}
