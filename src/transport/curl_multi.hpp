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

#include "curl_transfer.hpp"

#include <unordered_map>

namespace ym::detail {

struct CurlMultiOwner {
    static std::expected<std::unique_ptr<CurlMultiOwner>, ym_error> create(CurlPolicy) noexcept;
    ~CurlMultiOwner();
    CurlMultiOwner(const CurlMultiOwner&) = delete;
    CurlMultiOwner& operator=(const CurlMultiOwner&) = delete;
    std::expected<void, ym_error> start(uint64_t, const RequestInput&) noexcept;
    void cancel(uint64_t) noexcept;
    std::shared_ptr<CurlRuntime> runtime;
    CurlPolicy policy;
    CURLM* handle{nullptr};
    bool failed{false};
    std::unordered_map<uint64_t, std::unique_ptr<CurlTransfer>> active;

private:
    CurlMultiOwner(std::shared_ptr<CurlRuntime> r, CurlPolicy p)
        : runtime{std::move(r)}
        , policy{p}
    {
    }
};

} // namespace ym::detail
