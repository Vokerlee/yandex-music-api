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
#include "curl_adapter.hpp"

#include <algorithm>
#include <new>

namespace ym::detail {
namespace {

ym_status classify(CURLcode code) noexcept
{
    switch (code) {
    case CURLE_OK:
        return YM_OK;
    case CURLE_OUT_OF_MEMORY:
        return YM_OUT_OF_MEMORY;
    case CURLE_OPERATION_TIMEDOUT:
        return YM_DEADLINE_EXCEEDED;
    case CURLE_UNSUPPORTED_PROTOCOL:
        return YM_UNSUPPORTED;
    case CURLE_URL_MALFORMAT:
        return YM_INVALID_ARGUMENT;
    case CURLE_SSL_CONNECT_ERROR:
    case CURLE_PEER_FAILED_VERIFICATION:
    case CURLE_SSL_CERTPROBLEM:
    case CURLE_SSL_CACERT_BADFILE:
    case CURLE_SSL_ISSUER_ERROR:
    case CURLE_SSL_CRL_BADFILE:
    case CURLE_SSL_PINNEDPUBKEYNOTMATCH:
        return YM_TLS_ERROR;
    default:
        return YM_NETWORK_ERROR;
    }
}

class CurlAdapter final : public Transport {
public:
    explicit CurlAdapter(std::unique_ptr<CurlMultiOwner> owner)
        : owner_{std::move(owner)}
    {
    }

    void start(uint64_t ticket, const RequestInput& input) override
    {
        auto result = owner_->start(ticket, input);
        if (!result) {
            if (result.error().code == YM_OUT_OF_MEMORY)
                throw std::bad_alloc();
            throw std::runtime_error("Could not start Curl transfer");
        }
    }

    void cancel(uint64_t ticket) noexcept override { owner_->cancel(ticket); }

    std::optional<TransportResponse> poll(uint64_t) override
    {
        if (!owner_->failed) {
            int running = 0;
            if (curl_multi_perform(owner_->handle, &running) != CURLM_OK)
                owner_->failed = true;
        }
        if (owner_->failed) {
            return poll_failure();
        }
        int remaining = 0;
        while (auto* message = curl_multi_info_read(owner_->handle, &remaining)) {
            if (message->msg != CURLMSG_DONE)
                continue;
            if (auto response = complete_transfer(*message))
                return response;
        }
        return std::nullopt;
    }

private:
    std::optional<TransportResponse> poll_failure()
    {
        if (owner_->active.empty())
            return std::nullopt;
        TransportResponse response;
        response.ticket = owner_->active.begin()->first;
        response.status = YM_NETWORK_ERROR;
        response.http_status = 0;
        owner_->cancel(response.ticket);
        return response;
    }

    std::optional<TransportResponse> complete_transfer(const CURLMsg& message)
    {
        const auto found = std::find_if(owner_->active.begin(), owner_->active.end(),
            [&](const auto& item) { return item.second->handle == message.easy_handle; });
        // Do not dereference a queued handle which was already cancelled.
        if (found == owner_->active.end())
            return std::nullopt;
        auto& transfer = *found->second;
        TransportResponse response;
        response.ticket = found->first;
        response.status = transfer.response.failure != YM_OK ? transfer.response.failure
                                                             : classify(message.data.result);
        long status = 0;
        curl_easy_getinfo(transfer.handle, CURLINFO_RESPONSE_CODE, &status);
        response.http_status = status >= 0 && status <= 999 ? static_cast<uint32_t>(status) : 0;
        response.body = std::move(transfer.response.body);
        response.request_id = std::move(transfer.response.request_id);
        response.retry_after_ms = transfer.response.retry_after_ms;
        owner_->cancel(response.ticket);
        return response;
    }

    std::unique_ptr<CurlMultiOwner> owner_;
};

} // namespace

std::expected<std::shared_ptr<Transport>, ym_error> make_curl_transport(CurlPolicy policy) noexcept
{
    auto owner = CurlMultiOwner::create(policy);
    if (!owner)
        return std::unexpected(owner.error());
    ym_error error;
    try {
        return std::shared_ptr<Transport>(std::make_shared<CurlAdapter>(std::move(*owner)));
    } catch (const std::bad_alloc&) {
        fail(&error, YM_OUT_OF_MEMORY, "Transport allocation failed");
    } catch (...) {
        fail(&error, YM_INTERNAL_ERROR, "Transport initialization failed");
    }
    return std::unexpected(error);
}

} // namespace ym::detail
