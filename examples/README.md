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
| [search/tracks/main.c](search/tracks/main.c) | [search/tracks/main.cpp](search/tracks/main.cpp) | Search tracks by name and print matching IDs and titles | `QUERY` |
| [search/artists/main.c](search/artists/main.c) | [search/artists/main.cpp](search/artists/main.cpp) | Search artists by name and print matching IDs and titles | `QUERY` |
| [search/albums/main.c](search/albums/main.c) | [search/albums/main.cpp](search/albums/main.cpp) | Search albums by name and print matching IDs and titles | `QUERY` |
| [search/playlists/main.c](search/playlists/main.c) | [search/playlists/main.cpp](search/playlists/main.cpp) | Search playlists by name and print matching IDs and titles | `QUERY` |
| [search/podcasts/main.c](search/podcasts/main.c) | [search/podcasts/main.cpp](search/podcasts/main.cpp) | Search podcasts by name and print matching IDs and titles | `QUERY` |
| [search/episodes/main.c](search/episodes/main.c) | [search/episodes/main.cpp](search/episodes/main.cpp) | Search episodes by name and print matching IDs and titles | `QUERY` |
| [search/suggestions/main.c](search/suggestions/main.c) | [search/suggestions/main.cpp](search/suggestions/main.cpp) | Request search suggestions | `PREFIX` |
| [tracks/metadata/main.c](tracks/metadata/main.c) | [tracks/metadata/main.cpp](tracks/metadata/main.cpp) | Fetch track metadata for comma-separated IDs | `TRACK_IDS` |
| [tracks/similar/main.c](tracks/similar/main.c) | [tracks/similar/main.cpp](tracks/similar/main.cpp) | List tracks recommended as similar to a track | `TRACK_ID` |
| [tracks/supplement/main.c](tracks/supplement/main.c) | [tracks/supplement/main.cpp](tracks/supplement/main.cpp) | Read the available track supplement description | `TRACK_ID` |
| [artists/info/main.c](artists/info/main.c) | [artists/info/main.cpp](artists/info/main.cpp) | Read an artist profile | `ARTIST_ID` |
| [artists/tracks/main.c](artists/tracks/main.c) | [artists/tracks/main.cpp](artists/tracks/main.cpp) | List the first page of an artist's tracks | `ARTIST_ID` |
| [artists/albums/main.c](artists/albums/main.c) | [artists/albums/main.cpp](artists/albums/main.cpp) | List the first page of an artist's albums | `ARTIST_ID` |
| [artists/similar/main.c](artists/similar/main.c) | [artists/similar/main.cpp](artists/similar/main.cpp) | Find artists similar to an artist | `ARTIST_ID` |
