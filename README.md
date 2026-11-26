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
