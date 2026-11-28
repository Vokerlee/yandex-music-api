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

#include <memory>
#include <string_view>

#include <yandex_music/account.h>
#include <yandex_music/api.h>
#include <yandex_music/catalogue.h>
#include <yandex_music/client.h>
#include <yandex_music/download.h>
#include <yandex_music/error.hpp>
#include <yandex_music/playlists.h>
#include <yandex_music/recommendations.h>
#include <yandex_music/request.hpp>
#include <yandex_music/search.h>
#include <yandex_music/wave.h>

namespace ym {

enum class LibraryType : uint32_t {
    track = YM_LIBRARY_TRACK,
    artist = YM_LIBRARY_ARTIST,
    album = YM_LIBRARY_ALBUM,
    playlist = YM_LIBRARY_PLAYLIST,
};

enum class SearchType : uint32_t {
    all = YM_SEARCH_ALL,
    track = YM_SEARCH_TRACK,
    artist = YM_SEARCH_ARTIST,
    album = YM_SEARCH_ALBUM,
    playlist = YM_SEARCH_PLAYLIST,
    user = YM_SEARCH_USER,
    podcast = YM_SEARCH_PODCAST,
    podcast_episode = YM_SEARCH_PODCAST_EPISODE,
};

struct ClientOptions {
    std::string_view token{};
    std::string_view language{"ru"};
    size_t max_response_bytes{8 * 1024 * 1024};
    size_t max_pending_requests{64};
};

class Client {
public:
    Client(const Client&) = delete;
    Client& operator=(const Client&) = delete;

    Client(Client&&) noexcept = default;
    Client& operator=(Client&&) noexcept = default;
    ~Client() noexcept = default;

    [[nodiscard]] static Result<Client> create(ClientOptions input = {}) noexcept
    {
        ym_client_options options;
        ym_client_options_init(&options);
        options.token = {input.token.data(), input.token.size()};
        options.language = {input.language.data(), input.language.size()};
        options.max_response_bytes = input.max_response_bytes;
        options.max_pending_requests = input.max_pending_requests;

        ym_client* value = nullptr;
        ym_error failure;
        if (ym_client_create(&options, &value, &failure) != YM_OK)
            return std::unexpected(Error{failure});

        return Client{value};
    }

    [[nodiscard]] std::string_view language() const noexcept
    {
        const auto value = ym_client_language(handle_.get());
        return value.size ? std::string_view(value.data, value.size) : std::string_view{};
    }

    [[nodiscard]] Result<Request> request(const ym_api_call& call,
        const ym_request_options* options = nullptr) noexcept
    {
        ym_request* value = nullptr;
        ym_error failure;
        if (ym_client_request(handle_.get(), &call, options, &value, &failure) != YM_OK)
            return std::unexpected(Error{failure});
        return Request{value};
    }

    [[nodiscard]] Result<Request> search(std::string_view text, SearchType type = SearchType::all,
        uint32_t page = 0, const ym_request_options* options = nullptr) noexcept
    {
        return start(ym_search, options, view(text), static_cast<ym_search_type>(type), page);
    }

    [[nodiscard]] Result<Request> search_suggest(std::string_view text,
        const ym_request_options* options = nullptr) noexcept
    {
        return start(ym_search_suggest, options, view(text));
    }

    [[nodiscard]] Result<Request> tracks(std::string_view ids,
        const ym_request_options* options = nullptr) noexcept
    {
        return start(ym_tracks, options, view(ids));
    }

    [[nodiscard]] Result<Request> track_similar(std::string_view track_id,
        const ym_request_options* options = nullptr) noexcept
    {
        return start(ym_track_similar, options, view(track_id));
    }

    [[nodiscard]] Result<Request> track_supplement(std::string_view track_id,
        const ym_request_options* options = nullptr) noexcept
    {
        return start(ym_track_supplement, options, view(track_id));
    }

    [[nodiscard]] Result<Request> artist(std::string_view id,
        const ym_request_options* options = nullptr) noexcept
    {
        return start(ym_artist, options, view(id));
    }

    [[nodiscard]] Result<Request> artist_tracks(std::string_view id, uint32_t page = 0,
        uint32_t page_size = 50, const ym_request_options* options = nullptr) noexcept
    {
        return start(ym_artist_tracks, options, view(id), page, page_size);
    }

    [[nodiscard]] Result<Request> artist_albums(std::string_view id, uint32_t page = 0,
        uint32_t page_size = 50, const ym_request_options* options = nullptr) noexcept
    {
        return start(ym_artist_albums, options, view(id), page, page_size);
    }

    [[nodiscard]] Result<Request> artist_similar(std::string_view id,
        const ym_request_options* options = nullptr) noexcept
    {
        return start(ym_artist_similar, options, view(id));
    }

    [[nodiscard]] Result<Request> albums(std::string_view ids,
        const ym_request_options* options = nullptr) noexcept
    {
        return start(ym_albums, options, view(ids));
    }

    [[nodiscard]] Result<Request> album_tracks(std::string_view id,
        const ym_request_options* options = nullptr) noexcept
    {
        return start(ym_album_tracks, options, view(id));
    }

    [[nodiscard]] Result<Request>
    account_status(const ym_request_options* options = nullptr) noexcept
    {
        return start(ym_account_status, options);
    }

    [[nodiscard]] Result<Request> likes(std::string_view user_id,
        LibraryType type = LibraryType::track, const ym_request_options* options = nullptr) noexcept
    {
        return start(ym_likes, options, view(user_id), static_cast<ym_library_type>(type));
    }

    [[nodiscard]] Result<Request> set_like(std::string_view user_id, LibraryType type,
        std::string_view ids, bool liked = true,
        const ym_request_options* options = nullptr) noexcept
    {
        return start(ym_set_like, options, view(user_id), static_cast<ym_library_type>(type),
            view(ids), static_cast<uint32_t>(liked));
    }

    [[nodiscard]] Result<Request> playlists(std::string_view user_id,
        const ym_request_options* options = nullptr) noexcept
    {
        return start(ym_playlists, options, view(user_id));
    }

    [[nodiscard]] Result<Request> playlist(std::string_view user_id, std::string_view kind,
        const ym_request_options* options = nullptr) noexcept
    {
        return start(ym_playlist, options, view(user_id), view(kind));
    }

    [[nodiscard]] Result<Request> playlist_recommendations(std::string_view user_id,
        std::string_view kind, const ym_request_options* options = nullptr) noexcept
    {
        return start(ym_playlist_recommendations, options, view(user_id), view(kind));
    }

    [[nodiscard]] Result<Request> playlist_create(std::string_view user_id, std::string_view title,
        std::string_view visibility = "private",
        const ym_request_options* options = nullptr) noexcept
    {
        return start(ym_playlist_create, options, view(user_id), view(title), view(visibility));
    }

    [[nodiscard]] Result<Request> playlist_delete(std::string_view user_id, std::string_view kind,
        const ym_request_options* options = nullptr) noexcept
    {
        return start(ym_playlist_delete, options, view(user_id), view(kind));
    }

    [[nodiscard]] Result<Request>
    recommendations(const ym_request_options* options = nullptr) noexcept
    {
        return start(ym_recommendations, options);
    }

    [[nodiscard]] Result<Request> personal_playlist(std::string_view id = "daily",
        const ym_request_options* options = nullptr) noexcept
    {
        return start(ym_personal_playlist, options, view(id));
    }

    [[nodiscard]] Result<Request> wave_start(std::string_view seed = "user:onyourwave",
        const ym_request_options* options = nullptr) noexcept
    {
        return start(ym_wave_start, options, view(seed));
    }

    [[nodiscard]] Result<Request> wave_next(std::string_view session_id,
        std::string_view queue = {}, const ym_request_options* options = nullptr) noexcept
    {
        return start(ym_wave_next, options, view(session_id), view(queue));
    }

    [[nodiscard]] Result<Request> wave_feedback(std::string_view session_id, std::string_view event,
        std::string_view track_id = {}, double played_seconds = 0, std::string_view batch_id = {},
        const ym_request_options* options = nullptr) noexcept
    {
        return start(ym_wave_feedback, options, view(session_id), view(event), view(track_id),
            played_seconds, view(batch_id));
    }

    [[nodiscard]] Result<Request> track_download_info(std::string_view id,
        const ym_request_options* options = nullptr) noexcept
    {
        return start(ym_track_download_info, options, view(id));
    }

    [[nodiscard]] Result<Request> track_file_info(std::string_view id,
        std::string_view quality = "hq", std::string_view codecs = "mp3",
        const ym_request_options* options = nullptr) noexcept
    {
        return start(ym_track_file_info, options, view(id), view(quality), view(codecs));
    }

    [[nodiscard]] Result<void> download_track(std::string_view id, std::string_view filename,
        std::string_view quality = "hq", std::string_view codecs = "mp3",
        uint64_t timeout_ms = 30000, ym_download_progress_fn progress = nullptr,
        void* userdata = nullptr) noexcept
    {
        ym_error failure;
        if (ym_track_download(handle_.get(), view(id), view(filename), view(quality), view(codecs),
                timeout_ms, progress, userdata, &failure) != YM_OK)
            return std::unexpected(Error{failure});
        return {};
    }

    [[nodiscard]] ym_client* native_handle() const noexcept { return handle_.get(); }

    [[nodiscard]] Result<void> poll() noexcept { return poll(monotonic_ms()); }

    [[nodiscard]] Result<void> poll(uint64_t now_ms, size_t budget = 64) noexcept
    {
        ym_error failure;
        if (ym_client_poll(handle_.get(), now_ms, budget, &failure) != YM_OK)
            return std::unexpected(Error{failure});
        return {};
    }

    [[nodiscard]] Result<Document> wait(Request& request, uint64_t timeout_ms = 30000) noexcept
    {
        ym_result* value = nullptr;
        ym_error failure;
        if (ym_client_wait(handle_.get(), request.native_handle(), timeout_ms, &value, &failure) !=
            YM_OK)
            return std::unexpected(Error{failure});
        return Document{value};
    }

    [[nodiscard]] Result<Document> wait(Result<Request> request,
        uint64_t timeout_ms = 30000) noexcept
    {
        if (!request)
            return std::unexpected(request.error());
        return wait(*request, timeout_ms);
    }

    [[nodiscard]] Result<void> close() noexcept
    {
        ym_error failure;
        if (ym_client_close(handle_.get(), &failure) != YM_OK)
            return std::unexpected(Error{failure});
        return {};
    }

private:
    static constexpr ym_string_view view(std::string_view input) noexcept
    {
        return {input.data(), input.size()};
    }

    template<class Function, class... Args>
    Result<Request> start(Function function, const ym_request_options* options,
        Args... args) noexcept
    {
        ym_request* value = nullptr;
        ym_error failure;
        if (function(handle_.get(), args..., options, &value, &failure) != YM_OK)
            return std::unexpected(Error{failure});
        return Request{value};
    }

    struct Deleter {
        void operator()(ym_client* value) const noexcept { ym_client_destroy(value); }
    };

    explicit Client(ym_client* value) noexcept
        : handle_{value}
    {
    }

    std::unique_ptr<ym_client, Deleter> handle_;
};

} // namespace ym
