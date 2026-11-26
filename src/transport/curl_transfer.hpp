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

#include "core/transport.hpp"
#include "curl_runtime.hpp"
#include "response_sink.hpp"

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#endif

#include <stdexcept>

#include <curl/curl.h>

namespace ym::detail {

struct CurlPolicy {
    size_t max_body{8 * 1024 * 1024};
    // Private loopback-only seam for synthetic HTTP tests. Public clients require TLS.
    bool allow_loopback_http{false};
};

struct CurlTransfer {
    CurlTransfer(std::shared_ptr<CurlRuntime> r, size_t limit)
        : runtime{std::move(r)}
        , response{limit}
    {
    }

    ~CurlTransfer();
    CurlTransfer(const CurlTransfer&) = delete;
    CurlTransfer& operator=(const CurlTransfer&) = delete;
    std::shared_ptr<CurlRuntime> runtime;
    ResponseSink response;
    CURL* handle{nullptr};
    curl_slist* headers{nullptr};
    uint64_t ticket{0};
};

void configure_request(CurlTransfer&, const RequestInput&);

template<class T>
void curl_option(CURL* handle, CURLoption option, T value)
{
    if (curl_easy_setopt(handle, option, value) != CURLE_OK)
        throw std::runtime_error("Curl option configuration failed");
}

std::expected<std::unique_ptr<CurlTransfer>, ym_error>
prepare_transfer(std::shared_ptr<CurlRuntime>, const RequestInput&, const CurlPolicy&) noexcept;

} // namespace ym::detail
