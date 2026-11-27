# Yandex Music Native

An unofficial Yandex Music library for C and C++.

**Development status:** the current public API supports client setup and reading API responses. It does not yet expose methods for making music-service requests.

## Add it to your application

Requirements: CMake 3.28 or newer and a compiler with C++23 support. On Linux and macOS, OpenSSL development headers are needed when building libcurl from source. Windows uses its native TLS support.

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
#include <iostream>

#include <yandex_music/yandex_music.hpp>

int main()
{
    auto client = ym::Client::create({.token = "YOUR_ACCESS_TOKEN"});
    if (!client) {
        std::cerr << client.error().message() << '\n';
        return 1;
    }
}
```

Pass a Yandex Music OAuth access token when creating the client. Keep tokens outside source control.

## Read an API response

You can read saved or externally obtained JSON responses:

```cpp
auto response = ym::parse_response(R"({"result":{"title":"My playlist"}})");
if (!response) {
    std::cerr << response.error().message() << '\n';
    return 1;
}

auto title = response->root().member("title");
if (!title)
    return 1;

auto text = title->string();
if (text)
    std::cout << *text << '\n';
```

Responses own their data. Keep the response alive while using its text and list views.

C++ operations report failures through `ym::Result<T>` rather than throwing exceptions. C applications include `<yandex_music/yandex_music.h>` and check the returned `ym_status`; `ym_error` provides the error message and HTTP status.

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
