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

#include "api_support.hpp"
#include "service/download_file.hpp"

#include <filesystem>
#include <fstream>

namespace {

std::string read(const std::filesystem::path& file)
{
    std::ifstream input{file, std::ios::binary};
    return {std::istreambuf_iterator<char>{input}, std::istreambuf_iterator<char>{}};
}

int YM_CALL cancel(uint64_t received, uint64_t, void*)
{
    return received >= 65536;
}

} // namespace

int main(int argc, char** argv)
{
    CHECK(argc == 3);
    const std::string origin{argv[1]};
    const std::filesystem::path destination{argv[2]};
    const auto filename = destination.string();
    auto partial = destination;
    partial += ".part";
    ym_error failure;
    auto save = [&](std::string_view endpoint, ym_download_progress_fn progress = nullptr) {
        return ym::detail::download_file(origin + std::string{endpoint}, filename, 10000, progress,
            nullptr, &failure, true);
    };
    CHECK(save("/audio") == YM_OK);
    CHECK(std::filesystem::file_size(destination) == 9 * 1024 * 1024 + 7);
    auto content = read(destination);
    CHECK(content.starts_with(std::string{"\0MUSIC\xff", 7}));
    CHECK(!std::filesystem::exists(partial));

    CHECK(save("/audio", cancel) == YM_CANCELLED);
    CHECK(read(destination) == content && !std::filesystem::exists(partial));
    CHECK(save("/truncated") == YM_NETWORK_ERROR);
    CHECK(read(destination) == content && !std::filesystem::exists(partial));
    CHECK(!ym::download_file(origin + "/audio", filename));

    ApiFixture fixture;
    fixture.transport->body =
        R"({"result":{"downloadInfo":{"transport":"encraw","url":"https://storage.example/track"}}})";
    auto encrypted = fixture.client.download_track("42", filename);
    CHECK(!encrypted && encrypted.error().code() == YM_UNSUPPORTED);
    CHECK(read(destination) == content);
}
