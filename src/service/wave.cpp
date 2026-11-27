/*
 * Endpoint behavior references MarshalX/yandex-music-api at
 * 9ca132e5c18a72aab779c1fc68c169380a9357d0:
 * yandex_music/_client/rotor_sessions.py and rotor/session_event.py (radio sessions).
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

#include "core/api.hpp"

#include <chrono>
#include <cmath>
#include <ctime>

#include <yandex_music/wave.h>

namespace {

std::string timestamp()
{
    const auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::tm utc{};
#ifdef _WIN32
    gmtime_s(&utc, &now);
#else
    gmtime_r(&now, &utc);
#endif
    char value[32]{};
    std::strftime(value, sizeof(value), "%Y-%m-%dT%H:%M:%SZ", &utc);
    return value;
}

void post(ym::detail::wire::RequestSpec& spec)
{
    spec.verb = ym::detail::wire::HttpMethod::post;
    spec.encoding = ym::detail::wire::BodyKind::json;
    spec.mutating = true;
}

} // namespace

ym_status YM_CALL ym_wave_start(ym_client* client, ym_string_view seed,
    const ym_request_options* options, ym_request** out, ym_error* error) YM_NOEXCEPT
{
    return ym::detail::start_api(
        client,
        [&](auto& spec) {
            spec.path = {"rotor", "session", "new"};
            post(spec);
            spec.body = {
                {"seeds",                   ym_value{ym_value::Array{ym_value{ym::detail::identifier(seed)}}}},
                {"includeTracksInResponse", ym_value{true}                                                   }
            };
        },
        options, out, error
    );
}

ym_status YM_CALL ym_wave_next(ym_client* client, ym_string_view session,
    ym_string_view input_queue, const ym_request_options* options, ym_request** out,
    ym_error* error) YM_NOEXCEPT
{
    return ym::detail::start_api(
        client,
        [&](auto& spec) {
            spec.path = {"rotor", "session", ym::detail::identifier(session), "tracks"};
            post(spec);
            ym_value::Array queue;
            const auto queue_text = ym::detail::text(input_queue);
            std::string_view remaining{queue_text};
            while (!remaining.empty()) {
                const auto end = remaining.find(',');
                auto id = remaining.substr(0, end);
                queue.emplace_back(ym::detail::identifier({id.data(), id.size()}));
                if (end == std::string_view::npos)
                    break;
                remaining.remove_prefix(end + 1);
            }
            spec.body = {
                {"queue", ym_value{std::move(queue)}}
            };
        },
        options, out, error
    );
}

ym_status YM_CALL ym_wave_feedback(ym_client* client, ym_string_view session,
    ym_string_view input_event, ym_string_view track, double seconds, ym_string_view batch,
    const ym_request_options* options, ym_request** out, ym_error* error) YM_NOEXCEPT
{
    return ym::detail::start_api(
        client,
        [&](auto& spec) {
            auto type = ym::detail::identifier(input_event);
            if (!std::isfinite(seconds) || seconds < 0)
                throw std::invalid_argument("Played seconds must be finite and nonnegative");
            spec.path = {"rotor", "session", ym::detail::identifier(session), "feedback"};
            post(spec);
            ym_value::Object event{
                {"type",      ym_value{type}       },
                {"timestamp", ym_value{timestamp()}}
            };
            if (type != "radioStarted")
                event.emplace_back("trackId", ym_value{ym::detail::identifier(track)});
            if (type == "trackFinished" || type == "skip" || type == "dislike")
                event.emplace_back("totalPlayedSeconds", ym_value{seconds});
            spec.body = {
                {"event", ym_value{std::move(event)}}
            };
            if (batch.size)
                spec.body.push_back(ym::detail::field("batchId", ym::detail::text(batch)));
        },
        options, out, error
    );
}
