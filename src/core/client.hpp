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

#pragma once
#include "engine.hpp"
#include <string>
#include <yandex_music/client.h>

struct ym_client {
    std::shared_ptr<ym::detail::engine> execution = std::make_shared<ym::detail::engine>();
    bool destroy_requested = false;
    std::string token;
    std::string language;
    size_t max_response_bytes = 0;
    size_t max_pending_requests = 0;
};
