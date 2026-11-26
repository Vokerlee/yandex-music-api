# Yandex Music Native

Unofficial Yandex Music SDK with a C++23 implementation and a C11-compatible API.
This initial component exposes SDK version/build information without network calls.

```cmake
add_subdirectory(external/yandex-music-api)
target_link_libraries(my_app PRIVATE yandex_music::c)
```

Include `<yandex_music/yandex_music.h>` and call `ym_version()`.
C consumers enable `LANGUAGES C CXX` so CMake supplies the implementation runtime.
Build with `cmake -S . -B build`, `cmake --build build`, then `ctest --test-dir build`.
Use `BUILD_SHARED_LIBS=ON` for a shared library. CMake 3.28+ and C++23 are required.
License: [GPLv3](LICENSE). Third-party notices: [LICENSES/THIRD_PARTY_NOTICES.md](LICENSES/THIRD_PARTY_NOTICES.md).

C++ applications link `yandex_music::cpp`, include `<yandex_music/yandex_music.hpp>`,
and call `ym::version()` / `ym::build_info()`. The target supplies C++23.

Install with `cmake --install build --prefix /prefix`. Applications then use
`find_package(yandex_music CONFIG REQUIRED)` and link the same target.

Private JSON dependency resolution supports AUTO, SYSTEM (never downloads), and FETCH.
The fallback JSON 3.12.0 archive is pinned by SHA-256 in cmake/dependencies.cmake.
Installed consumers do not need JSON headers.

The implemented foundations provide owned C error/client/result/request handles and
C++23 RAII/expected facades. The request engine is caller-driven and uses a private
transport seam; service endpoints and the real HTTP adapter are separate additions.
Offline response decoding supports bounded JSON, explicit root/result selection,
HTTP/service errors and owned metadata. Unknown wire fields are preserved.
Reference-derived work credits Ilya (Marshal); see LICENSES/THIRD_PARTY_NOTICES.md. No endpoint or full Python parity is claimed yet.

The private curl resolver accepts `YM_CURL_DEPENDENCY_MODE=AUTO|SYSTEM|FETCH`.
It reuses a parent `CURL::libcurl` target, otherwise AUTO searches before fetching
the pinned source. SYSTEM never downloads. The fetched build uses native Windows
TLS or OpenSSL on Unix; install OpenSSL development headers on Unix.

For development, install `requirements-dev.txt` and run `python3 scripts/format.py`
(or `--check` to verify formatting).
