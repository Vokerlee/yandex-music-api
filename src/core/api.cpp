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

#include "api.hpp"
#include "protocol/json_read.hpp"

#include <array>

#include <yandex_music/api.h>

namespace {

ym::detail::wire::HttpMethod method(ym_string_view value)
{
    using Method = ym::detail::wire::HttpMethod;
    constexpr std::array methods{
        std::pair{"GET",    Method::get   },
        std::pair{"POST",   Method::post  },
        std::pair{"PUT",    Method::put   },
        std::pair{"DELETE", Method::remove},
        std::pair{"PATCH",  Method::patch },
        std::pair{"HEAD",   Method::head  },
    };
    const auto name = ym::detail::text(value);
    for (const auto& [name_, method_] : methods)
        if (name == name_)
            return method_;
    throw std::invalid_argument("Unsupported HTTP method");
}

void fields(std::vector<ym::detail::wire::RequestField>& out, const ym_api_field* input,
    size_t count)
{
    if (!input && count)
        throw std::invalid_argument("Fields are required");
    for (size_t i = 0; i < count; ++i)
        out.push_back(ym::detail::field(ym::detail::text(input[i].name),
            ym::detail::text(input[i].value)));
}

} // namespace

ym_status YM_CALL ym_client_request(ym_client* client, const ym_api_call* call,
    const ym_request_options* options, ym_request** out, ym_error* error) YM_NOEXCEPT
{
    return ym::detail::start_api(
        client,
        [&](auto& spec) {
            if (!call || !call->path || !call->path_count)
                throw std::invalid_argument("API path is required");
            spec.verb = method(call->method);
            spec.mutating = spec.verb != ym::detail::wire::HttpMethod::get &&
                spec.verb != ym::detail::wire::HttpMethod::head;
            for (size_t i = 0; i < call->path_count; ++i) {
                auto segment = ym::detail::text(call->path[i]);
                if (segment.empty() || segment == "." || segment == "..")
                    throw std::invalid_argument("Invalid API path segment");
                spec.path.push_back(std::move(segment));
            }
            fields(spec.query, call->query, call->query_count);
            fields(spec.body, call->form, call->form_count);
            if (call->json.size) {
                if (call->form_count)
                    throw std::invalid_argument("JSON and form bodies are mutually exclusive");
                auto parsed = ym::detail::read_json(ym::detail::text(call->json));
                if (!parsed)
                    throw ym::detail::ApiFailure{parsed.error()};
                spec.encoding = ym::detail::wire::BodyKind::json;
                spec.json_root = std::move(*parsed);
            } else if (call->form_count)
                spec.encoding = ym::detail::wire::BodyKind::form;
        },
        options, out, error
    );
}
