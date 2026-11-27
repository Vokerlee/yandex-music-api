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
#include "curl_runtime.hpp"

#include <mutex>
#include <new>
#include <string_view>

#include <curl/curl.h>

namespace ym::detail {
namespace {

std::mutex global_mutex;

bool supported_runtime(const curl_version_info_data* info) noexcept
{
    constexpr auto required = CURL_VERSION_SSL | CURL_VERSION_ASYNCHDNS | CURL_VERSION_THREADSAFE;
    if (!info || info->version_num < 0x080500 || (info->features & required) != required ||
        !info->protocols)
        return false;

    for (auto protocol = info->protocols; *protocol; ++protocol)
        if (std::string_view{*protocol} == "https")
            return true;

    return false;
}

} // namespace

std::expected<std::shared_ptr<CurlRuntime>, ym_error> CurlRuntime::create() noexcept
{
    ym_error error;
    try {
        // Construct before initializing so allocation failure cannot leak an init.
        std::shared_ptr<CurlRuntime> runtime(new CurlRuntime);
        {
            std::lock_guard lock(global_mutex);
            if (curl_global_init(CURL_GLOBAL_DEFAULT) != CURLE_OK) {
                fail(&error, YM_NETWORK_ERROR, "Curl initialization failed");
                return std::unexpected(error);
            }
            runtime->initialized_ = true;
        }
        if (!supported_runtime(curl_version_info(CURLVERSION_NOW))) {
            fail(&error, YM_UNSUPPORTED,
                "Curl requires HTTPS, asynchronous DNS and thread-safe initialization (8.5+)");
            return std::unexpected(error);
        }

        return runtime;
    } catch (const std::bad_alloc&) {
        fail(&error, YM_OUT_OF_MEMORY, "Curl runtime allocation failed");
        return std::unexpected(error);
    } catch (...) {
        fail(&error, YM_INTERNAL_ERROR, "Curl runtime initialization failed");
        return std::unexpected(error);
    }
}

CurlRuntime::~CurlRuntime()
{
    if (initialized_) {
        std::lock_guard lock(global_mutex);
        curl_global_cleanup();
    }
}

} // namespace ym::detail
