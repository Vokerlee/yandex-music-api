# Yandex Music Native

Unofficial Yandex Music SDK with a C++23 implementation, a C11-compatible interface,
and a C++23 convenience interface. This initial version exposes SDK version/build
information. Service operations will be added separately.

```cmake
add_subdirectory(external/yandex-music-api)
target_link_libraries(my_app PRIVATE yandex_music::cpp)
# C applications link yandex_music::c instead.
```

```cpp
#include <yandex_music/yandex_music.hpp>
auto version = ym::version();
```

C consumers include `<yandex_music/yandex_music.h>` and call `ym_version()`.
Source builds need a C++23 compiler even for C applications; use `LANGUAGES C CXX`
in a C consumer's CMake project. Includes and runtime linkage come from the target.

Build and test with CMake 3.28+, GCC 13+ or an equivalent C++23 toolchain:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
ctest --test-dir build --output-on-failure --no-tests=error
```

`BUILD_SHARED_LIBS=ON` selects a shared library. SDK tests default off when used
as a subproject. The library is distributed under the [GNU GPLv3](LICENSE).

Install with `cmake --install build --prefix /desired/prefix`. Consumers then use
`find_package(yandex_music CONFIG REQUIRED)` and link the same `yandex_music::c`
or `yandex_music::cpp` target. Set `CMAKE_PREFIX_PATH` for a nonstandard prefix.
`YM_INSTALL` defaults on for standalone builds and off for subprojects. Static C
consumers enable both C and CXX; installed shared libraries also support C-only
projects. Installed headers and targets remain usable after relocating the prefix.

Dependencies are private. `YM_DEPENDENCY_MODE=AUTO` uses an existing compatible
`nlohmann_json` package or fetches the pinned 3.12.0 archive with SHA-256 verification.
`SYSTEM` never downloads; `FETCH` selects the pinned source unless the parent already
provides the target. Standard FetchContent source overrides and CMake dependency providers
are supported. The installed SDK does not require JSON headers in the application.
No test framework is downloaded. libcurl will be introduced with the HTTP adapter;
this version does not prepare unused networking or media libraries.

`ym::client::create({.token = token, .language = "en"})` returns
`std::expected<ym::client, ym::error>`. Clients own their copied configuration and
perform no network I/O on creation. C uses `ym_client_options_init`,
`ym_client_create` and `ym_client_destroy`. Errors are caller-owned bounded values,
including allocation failures; no exception or global last-error crosses the C ABI.
Options accept older complete prefixes and ignore unknown tail fields of version 1.
Destroying NULL is harmless; use one driving thread per client.

Results use independent immutable storage: `ym_result_destroy` releases C owners,
and C++ `ym::document` releases them through RAII. `clone()` creates an independent
copy. Nested value views borrow from that document, never from the client. Missing,
null, empty, zero and false remain distinct; signed/unsigned integers never pass
through floating point. Strong C++ resource IDs prevent mixing track/album/artist
identities. These are low-level response primitives, not implemented catalog endpoints.

The request engine is caller-driven: `ym_client_poll` / `client.poll` advances bounded
work using supplied monotonic milliseconds. Requests own copied inputs; results transfer
once. Cancellation detaches transport access before releasing inputs. Started mutations
can have an unknown server-side outcome. `close()` cancels and suppresses pending callbacks;
request handles remain queryable afterward. Poll/close inside a callback is rejected;
destruction inside a callback is deferred safely. Service submission and the real HTTP
adapter are introduced separately; this foundation is tested through a private fake adapter.

For development, `YM_ENABLE_ASAN=ON` instruments SDK/tests with ASan+UBSan;
`YM_ENABLE_TSAN=ON` selects a separate TSan build. Instrumented static SDKs require a
matching sanitizer runtime in their application. Normal exported targets carry neither
sanitizer nor SDK warning flags.
