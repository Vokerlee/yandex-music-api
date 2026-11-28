# C and C++ examples

Each directory contains neighboring `main.c` and `main.cpp` programs with the same behavior. They use only the public SDK headers and can be read independently.

Build them with:

```sh
cmake -S . -B build -DYM_BUILD_EXAMPLES=ON
cmake --build build --config Debug --parallel
```

Targets are named `ym_example_c_<directory>` and `ym_example_cpp_<directory>`, with `/` replaced by `_`. For example:

```sh
./build/ym_example_c_basics_version
./build/ym_example_cpp_basics_version
```

With a multi-configuration generator on Windows, run the corresponding executable in `build/Debug/`.

Network examples read `YANDEX_MUSIC_TOKEN`; obtain it using the [authorization guide](../docs/authentication.md). Pass the arguments shown below after the executable name.

| C program | C++ program | Behavior | Arguments |
| --- | --- | --- | --- |
| [basics/version/main.c](basics/version/main.c) | [basics/version/main.cpp](basics/version/main.cpp) | Print the SDK version and build information | `` |
| [basics/client/main.c](basics/client/main.c) | [basics/client/main.cpp](basics/client/main.cpp) | Create, configure, inspect and close a client | `` |
