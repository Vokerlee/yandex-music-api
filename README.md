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

For development, install `requirements-dev.txt` and run `python3 scripts/format.py`
(or `--check` to verify formatting).
