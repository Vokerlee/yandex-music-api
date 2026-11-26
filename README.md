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
License: [GPLv3](LICENSE).
