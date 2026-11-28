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

#include <charconv>
#include <iostream>
#include <string_view>

#include <yandex_music/cpp/yandex_music.hpp>

int YM_CALL progress(uint64_t received, uint64_t total, void* userdata)
{
    uint64_t limit = *static_cast<const uint64_t*>(userdata);
    std::cerr << '\r' << received << " / " << total << " bytes";
    return limit != 0 && received >= limit;
}

int main(int argc, char** argv)
{
    if (argc != 4) {
        std::cerr
            << "Usage: " << argv[0]
            << " HTTPS_URL FILENAME CANCEL_AFTER_BYTES\nUse 0 to download without a byte limit.\n";
        return 1;
    }
    uint64_t limit = 0;
    std::string_view text = argv[3];
    auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), limit);
    if (error != std::errc{} || end != text.data() + text.size()) {
        std::cerr << "CANCEL_AFTER_BYTES must be an unsigned 64-bit integer.\n";
        return 1;
    }

    auto saved = ym::download_file(argv[1], argv[2], 30000, progress, &limit);
    std::cerr << '\n';
    if (!saved && saved.error().code() == YM_CANCELLED) {
        std::cout << "Cancelled; partial output removed.\n";
        return 0;
    }
    if (!saved) {
        std::cerr << saved.error().message() << '\n';
        return 1;
    }
    std::cout << "Saved " << argv[2] << '\n';
}
