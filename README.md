# Yandex Music API

An unofficial Yandex Music library for C and C++.

**Development status:** under active development. The C and C++ interfaces support authenticated Yandex Music requests; API coverage is growing.

## Add it to your application

Requirements: CMake 3.28 or newer and a compiler with C++23 support. Linux and macOS also need the OpenSSL development package. Windows 10 or newer uses native TLS and cryptography.

Fetch the library from GitHub and link the C++ target:

```cmake
include(FetchContent)

FetchContent_Declare(yandex_music
    GIT_REPOSITORY https://github.com/Vokerlee/yandex-music-api.git
    GIT_TAG master
)
FetchContent_MakeAvailable(yandex_music)

target_link_libraries(my_app PRIVATE yandex_music::cpp)
```

You can also clone it into your project and use `add_subdirectory(external/yandex-music-api)`. Use a specific commit instead of `master` to pin the version.

For a C application, enable both languages in your project and link the C target:

```cmake
project(my_app LANGUAGES C CXX)
add_subdirectory(external/yandex-music-api)
target_link_libraries(my_app PRIVATE yandex_music::c)
```

The build finds or downloads its dependencies automatically.

## Create a client

```cpp
#include <cstdlib>
#include <iostream>

#include <yandex_music/cpp/yandex_music.hpp>

int main()
{
    const char* token = std::getenv("YANDEX_MUSIC_TOKEN");
    if (!token) {
        std::cerr << "Set YANDEX_MUSIC_TOKEN first\n";
        return 1;
    }

    auto client = ym::Client::create({.token = token});
    if (!client) {
        std::cerr << client.error().message() << '\n';
        return 1;
    }
}
```

Get an access token using the [English authorization guide](docs/authentication.md), then set `YANDEX_MUSIC_TOKEN`. Keep tokens outside source control.

## Search for music

```cpp
auto response = client->wait(client->search("Radiohead", ym::SearchType::track));
if (!response) {
    std::cerr << response.error().message() << '\n';
    return 1;
}

auto tracks = response->root()["tracks"]["results"];
for (size_t i = 0; i < tracks.size().value_or(0); ++i)
    std::cout << tracks[i]["title"].string().value_or("") << '\n';
```

Choose `SearchType::artist`, `album`, `playlist`, `podcast` or `podcast_episode` to search another category. Pages start at zero; pass the page as the third argument. Each category contains `results`, `total` and `perPage`.

Responses own their data. Keep the response alive while using its text and list views. `operator[]` returns an empty view for missing fields; typed accessors return a result that you can check or read with `value_or`.

C++ calls report errors through `ym::Result<T>` and do not throw exceptions. Use `client->wait(request)` for a blocking call, or keep the request and advance it with `client->poll(monotonic_ms)` in your event loop. Pending requests can be cancelled.

## Explore your music

Call these methods on your client, then pass the returned request to `wait` or poll it asynchronously.

| What you need | Client methods |
| --- | --- |
| Track details and similar music | `tracks("42,43")`, `track_similar("42")`, `track_supplement("42")` |
| Artist information, tracks and albums | `artist("7")`, `artist_tracks("7", 0, 50)`, `artist_albums("7")` |
| Album information and track lists | `albums("8,9")`, `album_tracks("8")` |
| Account and liked music | `account_status()`, `likes("USER_ID")`, `set_like("USER_ID", ym::LibraryType::track, "42")` |
| Account playlists and recommendations | `playlists("USER_ID")`, `playlist("USER_ID", "100")`, `playlist_recommendations("USER_ID", "100")` |
| Create or delete a playlist | `playlist_create("USER_ID", "Road trip")`, `playlist_delete("USER_ID", "100")` |
| Personal recommendations | `recommendations()`, `personal_playlist("daily")` |
| My Wave and seeded radio | `wave_start()`, `wave_next(session_id)`, `wave_feedback(session_id, event, track_id)` |
| Available audio formats and URLs | `track_download_info("42")`, `track_file_info("42", "hq", "mp3")` |
| Download audio to a local file | `download_track("42", "song.mp3")` |

Get your account ID from `account_status()` → `account.uid`. Pass it as a string wherever `USER_ID` appears. Use `LibraryType::artist`, `album` or `playlist` with `likes` and `set_like` for those collections.

## Listen to My Wave

```cpp
auto wave = client->wait(client->wave_start());
if (!wave)
    return 1;

auto session = wave->root()["radioSessionId"].string();
if (!session)
    return 1;

auto next = client->wait(client->wave_next(*session, "42:8,43"));
auto feedback = client->wait(client->wave_feedback(*session, "skip", "42:8", 12.5));
```

The response's `sequence` contains the suggested tracks. Pass the IDs you already received as `queue` to avoid repeats. Replace the sample IDs with tracks from your session. Report `radioStarted`, `trackStarted`, `trackFinished` and `skip` as playback progresses. You can also seed a station with an artist, track or genre, such as `wave_start("genre:rock")`.

## Save a local copy

```cpp
auto saved = client->download_track("TRACK_ID", "song.mp3");
if (!saved)
    std::cerr << saved.error().message() << '\n';
```

The default requests high-quality MP3. For another format, inspect `track_file_info(id, quality, codecs)` and choose a filename for the returned codec/container. Available formats and quality depend on the track and account; `quality` can be `nq`, `hq` or `lossless`.

This is a blocking call. For a GUI, use a separate client on a worker thread. Call a given client from one thread at a time. Downloads stream to disk and replace the destination after a successful transfer. You can supply a progress callback and cancel by returning nonzero. Encrypted audio delivery is not supported.

## Use the C API

Include `<yandex_music/c/yandex_music.h>` and link `yandex_music::c`. Functions return `ym_status`; an optional `ym_error` contains the message and HTTP status.

```c
ym_request* request = NULL;
ym_result* response = NULL;
ym_error error;

ym_status status = ym_search(client, (ym_string_view){"Radiohead", 9},
    YM_SEARCH_TRACK, 0, NULL, &request, &error);
if (status == YM_OK)
    status = ym_client_wait(client, request, 30000, &response, &error);

/* Read response with ym_result_root and ym_value_member/at/string. */
ym_result_destroy(response);
ym_request_destroy(request);
```

Create the C client with `ym_client_options_init` and `ym_client_create`, then release it with `ym_client_destroy`. Request inputs are copied, and results remain valid until you destroy them.

## Runnable examples

See the [paired C and C++ programs](examples/README.md). Every example has its own `main`; matching implementations live side by side.

```sh
cmake -S . -B build -DYM_BUILD_EXAMPLES=ON
cmake --build build --config Debug --parallel
```

## Install the library

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
cmake --install build --config Release --prefix /your/install/path
```

Set `CMAKE_PREFIX_PATH` to that location when configuring your application:

```cmake
find_package(yandex_music CONFIG REQUIRED)
target_link_libraries(my_app PRIVATE yandex_music::cpp)
```

Use `yandex_music::c` for C applications. Add `-DBUILD_SHARED_LIBS=ON` when configuring the library to build a shared library.

## License and credits

Licensed under [GPLv3](LICENSE). See [third-party notices](LICENSES/THIRD_PARTY_NOTICES.md) for component licenses and attribution.

API behavior is informed by [MarshalX/yandex-music-api](https://github.com/MarshalX/yandex-music-api).
