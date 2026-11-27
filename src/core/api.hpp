/*
 * Request identity references MarshalX/yandex-music-api at
 * 9ca132e5c18a72aab779c1fc68c169380a9357d0:
 * yandex_music/utils/request_base.py.
 * Copyright (C) 2019-2026 Ilya (Marshal) <https://github.com/MarshalX>
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

#include "client.hpp"
#include "error.hpp"
#include "protocol/encode.hpp"
#include "protocol/response.hpp"

#include <algorithm>
#include <new>
#include <stdexcept>

namespace ym::detail {

struct ApiFailure {
    ym_error error;
};

inline std::string text(ym_string_view value)
{
    if (!value.data && value.size)
        throw std::invalid_argument("Text data is required");
    return value.size ? std::string(value.data, value.size) : std::string{};
}

inline std::string identifier(ym_string_view value)
{
    auto result = text(value);
    if (result.empty() || result == "." || result == "..")
        throw std::invalid_argument("An identifier is required");
    return result;
}

inline wire::RequestField field(std::string name, std::string value)
{
    return {std::move(name), ym_value{std::move(value)}};
}

template<class Build>
ym_status start_api(ym_client* client, Build&& build, const ym_request_options* options,
    ym_request** out, ym_error* error) noexcept
{
    clear(error);
    if (out)
        *out = nullptr;
    if (!client || !out)
        return fail(error, YM_INVALID_ARGUMENT, "Client and request output are required");
    try {
        wire::RequestSpec spec;
        spec.origin = "https://api.music.yandex.net";
        build(spec);
        if (std::ranges::none_of(spec.headers,
                [](const auto& header) { return header.first == "X-Yandex-Music-Client"; }))
            spec.headers.emplace_back("X-Yandex-Music-Client", "YandexMusicAndroid/24023621");
        spec.headers.emplace_back("User-Agent", "Yandex-Music-Native");
        spec.headers.emplace_back("Accept-Language", client->language);
        if (!client->token.empty())
            spec.headers.emplace_back("Authorization", "OAuth " + client->token);

        auto input = wire::encode(spec);
        if (!input) {
            if (error)
                *error = input.error();
            return input.error().code;
        }
        return submit(client, std::move(*input), decode_response, options, out, error);
    } catch (const std::bad_alloc&) {
        return fail(error, YM_OUT_OF_MEMORY, "API request allocation failed");
    } catch (const ApiFailure& failure) {
        if (error)
            *error = failure.error;
        return failure.error.code;
    } catch (const std::invalid_argument& reason) {
        return fail(error, YM_INVALID_ARGUMENT, reason.what());
    } catch (...) {
        return fail(error, YM_INTERNAL_ERROR, "API request could not be prepared");
    }
}

} // namespace ym::detail
