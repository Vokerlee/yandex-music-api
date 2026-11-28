/*
 * Endpoint behavior references MarshalX/yandex-music-api at
 * 9ca132e5c18a72aab779c1fc68c169380a9357d0:
 * yandex_music/track/file_download_info.py (saving server-provided raw audio).
 * Copyright (C) 2019-2026 Ilya (Marshal) <https://github.com/MarshalX>
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
#include "download_file.hpp"
#include "transport/curl_transfer.hpp"

#include <cerrno>
#include <climits>
#include <cstdio>
#include <filesystem>
#include <limits>
#include <string>
#include <system_error>
#include <utility>

#ifdef _WIN32
#include <Windows.h>
#endif

namespace ym::detail {
namespace {

class OutputFile {
public:
    explicit OutputFile(std::string_view filename)
        : destination_{std::u8string(filename.begin(), filename.end())}
        , partial_{destination_}
    {
        partial_ += ".part";
#ifdef _WIN32
        _wfopen_s(&file_, partial_.c_str(), L"wbx");
#else
        file_ = std::fopen(partial_.c_str(), "wbx");
#endif
        if (!file_)
            throw std::system_error(errno, std::generic_category());
    }

    ~OutputFile()
    {
        if (file_)
            std::fclose(file_);
        if (!committed_) {
            std::error_code ignored;
            std::filesystem::remove(partial_, ignored);
        }
    }

    OutputFile(const OutputFile&) = delete;
    OutputFile& operator=(const OutputFile&) = delete;

    FILE* get() const noexcept { return file_; }

    void commit()
    {
        auto* file = std::exchange(file_, nullptr);
        if (std::fclose(file) != 0)
            throw std::system_error(errno, std::generic_category());
#ifdef _WIN32
        if (!MoveFileExW(partial_.c_str(), destination_.c_str(),
                MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
            throw std::system_error(static_cast<int>(GetLastError()), std::system_category());
#else
        std::filesystem::rename(partial_, destination_);
#endif
        committed_ = true;
    }

private:
    std::filesystem::path destination_;
    std::filesystem::path partial_;
    FILE* file_{nullptr};
    bool committed_{false};
};

size_t write_file(char* data, size_t size, size_t count, void* context) noexcept
{
    if (size && count > std::numeric_limits<size_t>::max() / size)
        return 0;
    return std::fwrite(data, 1, size * count, static_cast<FILE*>(context));
}

struct Progress {
    ym_download_progress_fn callback;
    void* userdata;
};

int progress(void* context, curl_off_t total, curl_off_t received, curl_off_t, curl_off_t) noexcept
{
    const auto& state = *static_cast<Progress*>(context);
    if (!state.callback)
        return 0;
    return state.callback(static_cast<uint64_t>(received), static_cast<uint64_t>(total),
        state.userdata);
}

ym_status transfer_status(CURLcode code, long http) noexcept
{
    if (code == CURLE_OPERATION_TIMEDOUT)
        return YM_DEADLINE_EXCEEDED;
    if (code == CURLE_ABORTED_BY_CALLBACK)
        return YM_CANCELLED;
    if (code == CURLE_WRITE_ERROR)
        return YM_IO_ERROR;
    if (code == CURLE_PEER_FAILED_VERIFICATION || code == CURLE_SSL_CONNECT_ERROR)
        return YM_TLS_ERROR;
    if (http >= 300) {
        switch (http) {
        case 401:
            return YM_UNAUTHORIZED;
        case 403:
            return YM_FORBIDDEN;
        case 404:
            return YM_NOT_FOUND;
        case 429:
            return YM_RATE_LIMITED;
        default:
            return YM_SERVICE_ERROR;
        }
    }
    return code == CURLE_OK && http >= 200 ? static_cast<ym_status>(YM_OK)
                                           : static_cast<ym_status>(YM_NETWORK_ERROR);
}

} // namespace

ym_status download_file(std::string_view url, std::string_view filename, uint64_t timeout_ms,
    ym_download_progress_fn callback, void* userdata, ym_error* error, bool loopback) noexcept
{
    clear(error);
    if (filename.empty() || filename.find('\0') != std::string_view::npos || !timeout_ms)
        return fail(error, YM_INVALID_ARGUMENT, "Filename and a positive timeout are required");
    try {
        auto runtime = CurlRuntime::create();
        if (!runtime) {
            if (error)
                *error = runtime.error();
            return runtime.error().code;
        }
        RequestInput input{.method = "GET", .url = std::string{url}};
        auto transfer = prepare_transfer(*runtime, input, {.allow_loopback_http = loopback});
        if (!transfer) {
            if (error)
                *error = transfer.error();
            return transfer.error().code;
        }
        OutputFile output{filename};
        Progress state{callback, userdata};
        auto* handle = (*transfer)->handle;
        curl_option(handle, CURLOPT_WRITEFUNCTION, write_file);
        curl_option(handle, CURLOPT_WRITEDATA, output.get());
        curl_option(handle, CURLOPT_FAILONERROR, 1L);
        curl_option(handle, CURLOPT_TIMEOUT_MS,
            timeout_ms > LONG_MAX ? LONG_MAX : static_cast<long>(timeout_ms));
        curl_option(handle, CURLOPT_XFERINFOFUNCTION, progress);
        curl_option(handle, CURLOPT_XFERINFODATA, &state);
        curl_option(handle, CURLOPT_NOPROGRESS, 0L);
        const auto code = curl_easy_perform(handle);
        long http = 0;
        curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &http);
        const auto status = transfer_status(code, http);
        if (status != YM_OK) {
            fail(error, status,
                code == CURLE_OK ? "Download HTTP request failed" : curl_easy_strerror(code));
            if (error)
                error->http_status = static_cast<uint32_t>(http);
            return status;
        }
        output.commit();
        return YM_OK;
    } catch (const std::bad_alloc&) {
        return fail(error, YM_OUT_OF_MEMORY, "Download allocation failed");
    } catch (const std::system_error& reason) {
        return fail(error, YM_IO_ERROR, reason.what());
    } catch (...) {
        return fail(error, YM_INTERNAL_ERROR, "Download could not be completed");
    }
}

} // namespace ym::detail
