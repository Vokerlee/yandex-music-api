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
#include "curl_multi.hpp"

#include <new>

namespace ym::detail {

std::expected<std::unique_ptr<CurlMultiOwner>, ym_error>
CurlMultiOwner::create(CurlPolicy policy) noexcept
{
    ym_error error;
    if (!policy.max_body) {
        fail(&error, YM_INVALID_ARGUMENT, "Response limit must be positive");
        return std::unexpected(error);
    }
    auto runtime = CurlRuntime::create();
    if (!runtime)
        return std::unexpected(runtime.error());
    try {
        std::unique_ptr<CurlMultiOwner> owner(new CurlMultiOwner(std::move(*runtime), policy));
        owner->handle = curl_multi_init();
        if (!owner->handle)
            throw std::bad_alloc();
        return owner;
    } catch (const std::bad_alloc&) {
        fail(&error, YM_OUT_OF_MEMORY, "Multi-handle allocation failed");
    } catch (...) {
        fail(&error, YM_INTERNAL_ERROR, "Multi-handle initialization failed");
    }
    return std::unexpected(error);
}

CurlMultiOwner::~CurlMultiOwner()
{
    if (handle) {
        for (const auto& entry : active)
            curl_multi_remove_handle(handle, entry.second->handle);
        curl_multi_cleanup(handle);
    }
    // Easy handles and borrowed bodies are detached before their owners release.
    active.clear();
}

std::expected<void, ym_error> CurlMultiOwner::start(uint64_t ticket,
    const RequestInput& input) noexcept
{
    ym_error error;
    if (!ticket || active.contains(ticket)) {
        fail(&error, YM_INVALID_ARGUMENT, "Transfer ticket must be unique and nonzero");
        return std::unexpected(error);
    }
    if (failed || !handle) {
        fail(&error, YM_NETWORK_ERROR, "Multi-handle is unavailable");
        return std::unexpected(error);
    }
    auto transfer = prepare_transfer(runtime, input, policy);
    if (!transfer)
        return std::unexpected(transfer.error());
    (*transfer)->ticket = ticket;
    try {
        const auto position = active.emplace(ticket, std::move(*transfer)).first;
        const auto code = curl_multi_add_handle(handle, position->second->handle);
        if (code != CURLM_OK) {
            cancel(ticket);
            fail(&error, code == CURLM_OUT_OF_MEMORY ? YM_OUT_OF_MEMORY : YM_NETWORK_ERROR,
                "Could not attach transfer");
            return std::unexpected(error);
        }
        return {};
    } catch (const std::bad_alloc&) {
        fail(&error, YM_OUT_OF_MEMORY, "Transfer registry allocation failed");
    } catch (...) {
        fail(&error, YM_INTERNAL_ERROR, "Transfer registry failed");
    }
    return std::unexpected(error);
}

void CurlMultiOwner::cancel(uint64_t ticket) noexcept
{
    const auto position = active.find(ticket);
    if (position == active.end())
        return;
    if (handle && curl_multi_remove_handle(handle, position->second->handle) != CURLM_OK) {
        // Even on an unexpected multi failure, no easy handle retains input access.
        curl_multi_cleanup(handle);
        handle = nullptr;
        failed = true;
    }
    active.erase(position);
}

} // namespace ym::detail
