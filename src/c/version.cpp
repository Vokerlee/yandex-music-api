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

#include <yandex_music/yandex_music.h>
#include "version_config.hpp"
#include <nlohmann/json.hpp>

static_assert(NLOHMANN_JSON_VERSION_MAJOR == 3 && NLOHMANN_JSON_VERSION_MINOR >= 12,
    "The SDK requires nlohmann_json >= 3.12");
#define YM_STRINGIFY_IMPL(value) #value
#define YM_STRINGIFY(value) YM_STRINGIFY_IMPL(value)

const char *YM_CALL ym_version(void)
{
    return YM_VERSION;
}

const char *YM_CALL ym_build_info(void)
{
    return "Yandex Music Native " YM_VERSION "; C++23 core; C ABI v1; JSON "
        YM_STRINGIFY(NLOHMANN_JSON_VERSION_MAJOR) "." YM_STRINGIFY(NLOHMANN_JSON_VERSION_MINOR)
        "." YM_STRINGIFY(NLOHMANN_JSON_VERSION_PATCH);
}
