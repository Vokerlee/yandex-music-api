/*
 * Endpoint behavior references MarshalX/yandex-music-api at
 * 9ca132e5c18a72aab779c1fc68c169380a9357d0:
 * yandex_music/utils/sign_request.py (desktop file-info HMAC-SHA256 signature).
 * Copyright (C) 2019-2026 Ilya (Marshal) <https://github.com/MarshalX>
 * Copyright (c) 2026 Vokerlee <vokerlee@gmail.com>
 *
 * Yandex Music Native is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3 as published
 * by the Free Software Foundation.
 *
 * Yandex Music Native is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "file_signature.hpp"

#include <array>
#include <cstdint>
#include <stdexcept>

#ifdef _WIN32
#include <Windows.h>
#include <bcrypt.h>
#else
#include <openssl/hmac.h>
#endif

namespace ym::detail {

std::string file_signature(std::string_view timestamp, std::string_view track_id,
    std::string_view quality, std::string_view codecs)
{
    std::string message{timestamp};
    message.append(track_id).append(quality);
    for (char c : codecs)
        if (c != ',')
            message.push_back(c);
    message.append("raw");
    constexpr std::string_view key{"kzqU4XhfCaY6B6JTHODeq5"};
    std::array<unsigned char, 32> digest{};
#ifdef _WIN32
    BCRYPT_ALG_HANDLE algorithm{};
    if (BCryptOpenAlgorithmProvider(&algorithm, BCRYPT_SHA256_ALGORITHM, nullptr,
            BCRYPT_ALG_HANDLE_HMAC_FLAG) < 0)
        throw std::runtime_error("HMAC provider is unavailable");
    const auto status =
        BCryptHash(algorithm, reinterpret_cast<PUCHAR>(const_cast<char*>(key.data())),
            static_cast<ULONG>(key.size()), reinterpret_cast<PUCHAR>(message.data()),
            static_cast<ULONG>(message.size()), digest.data(), static_cast<ULONG>(digest.size()));
    BCryptCloseAlgorithmProvider(algorithm, 0);
    if (status < 0)
        throw std::runtime_error("HMAC signing failed");
#else
    unsigned int length = 0;
    if (!HMAC(EVP_sha256(), key.data(), static_cast<int>(key.size()),
            reinterpret_cast<const unsigned char*>(message.data()), message.size(), digest.data(),
            &length))
        throw std::runtime_error("HMAC signing failed");
#endif
    constexpr std::string_view alphabet{
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"};
    std::string encoded;
    encoded.reserve(43);
    uint32_t bits = 0;
    unsigned count = 0;
    for (auto byte : digest) {
        bits = (bits << 8) | byte;
        count += 8;
        while (count >= 6) {
            count -= 6;
            encoded.push_back(alphabet[(bits >> count) & 63]);
        }
    }
    if (count)
        encoded.push_back(alphabet[(bits << (6 - count)) & 63]);
    return encoded;
}

} // namespace ym::detail
