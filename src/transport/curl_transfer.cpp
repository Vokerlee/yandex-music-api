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

#include "core/error.hpp"
#include "curl_transfer.hpp"

#include <memory>
#include <new>
#include <string_view>

namespace ym::detail {
namespace {

using UrlOwner = std::unique_ptr<CURLU, decltype(&curl_url_cleanup)>;
using CurlText = std::unique_ptr<char, decltype(&curl_free)>;

CurlText part(CURLU* url, CURLUPart kind)
{
    char* raw = nullptr;
    const auto code = curl_url_get(url, kind, &raw, 0);
    if (code == CURLUE_OUT_OF_MEMORY)
        throw std::bad_alloc();
    return CurlText(code == CURLUE_OK ? raw : nullptr, curl_free);
}

bool allowed_url(const std::string& value, bool loopback_http)
{
    if (value.find('\0') != std::string::npos)
        return false;
    UrlOwner url(curl_url(), curl_url_cleanup);
    if (!url)
        throw std::bad_alloc();
    const auto code = curl_url_set(url.get(), CURLUPART_URL, value.c_str(), 0);
    if (code == CURLUE_OUT_OF_MEMORY)
        throw std::bad_alloc();
    if (code != CURLUE_OK)
        return false;
    if (part(url.get(), CURLUPART_USER) || part(url.get(), CURLUPART_PASSWORD) ||
        part(url.get(), CURLUPART_FRAGMENT))
        return false;
    const auto scheme = part(url.get(), CURLUPART_SCHEME);
    const auto host = part(url.get(), CURLUPART_HOST);
    if (!scheme || !host)
        return false;
    if (std::string_view(scheme.get()) == "https")
        return true;
    return loopback_http && std::string_view(scheme.get()) == "http" &&
        (std::string_view(host.get()) == "127.0.0.1" || std::string_view(host.get()) == "[::1]");
}

void configure_handle(CurlTransfer& transfer, const RequestInput& input, const CurlPolicy& policy)
{
    auto* handle = transfer.handle;
    curl_option(handle, CURLOPT_URL, input.url.c_str());
    curl_option(handle, CURLOPT_PROTOCOLS_STR, policy.allow_loopback_http ? "http,https" : "https");
    curl_option(handle, CURLOPT_REDIR_PROTOCOLS_STR, "https");
    curl_option(handle, CURLOPT_FOLLOWLOCATION, 0L);
    curl_option(handle, CURLOPT_UNRESTRICTED_AUTH, 0L);
    curl_option(handle, CURLOPT_NETRC, static_cast<long>(CURL_NETRC_IGNORED));
    curl_option(handle, CURLOPT_PROXY, "");
    curl_option(handle, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_option(handle, CURLOPT_SSL_VERIFYHOST, 2L);
    curl_option(handle, CURLOPT_NOSIGNAL, 1L);
    curl_option(handle, CURLOPT_PATH_AS_IS, 1L);
    curl_option(handle, CURLOPT_CONNECTTIMEOUT_MS, 10000L);
    curl_option(handle, CURLOPT_ACCEPT_ENCODING, "");
    curl_option(handle, CURLOPT_WRITEFUNCTION, collect_body);
    curl_option(handle, CURLOPT_WRITEDATA, &transfer.response);
    curl_option(handle, CURLOPT_HEADERFUNCTION, collect_header);
    curl_option(handle, CURLOPT_HEADERDATA, &transfer.response);
    curl_option(handle, CURLOPT_PRIVATE, &transfer);
}

} // namespace

CurlTransfer::~CurlTransfer()
{
    if (handle)
        curl_easy_cleanup(handle);
    curl_slist_free_all(headers);
}

std::expected<std::unique_ptr<CurlTransfer>, ym_error> prepare_transfer(
    std::shared_ptr<CurlRuntime> runtime, const RequestInput& input, const CurlPolicy& policy
) noexcept
{
    ym_error error;
    try {
        if (!runtime || !policy.max_body || !allowed_url(input.url, policy.allow_loopback_http)) {
            fail(&error, YM_INVALID_ARGUMENT,
                "Transfer requires an HTTPS URL without userinfo or fragments");
            return std::unexpected(error);
        }

        auto transfer = std::make_unique<CurlTransfer>(std::move(runtime), policy.max_body);
        auto* handle = transfer->handle = curl_easy_init();
        if (!handle)
            throw std::bad_alloc();
        configure_handle(*transfer, input, policy);
        configure_request(*transfer, input);
        return transfer;
    } catch (const std::invalid_argument&) {
        fail(&error, YM_INVALID_ARGUMENT, "Invalid HTTP method, header or body");
    } catch (const std::bad_alloc&) {
        fail(&error, YM_OUT_OF_MEMORY, "Transfer allocation failed");
    } catch (...) {
        fail(&error, YM_NETWORK_ERROR, "Transfer configuration failed");
    }
    return std::unexpected(error);
}

} // namespace ym::detail
