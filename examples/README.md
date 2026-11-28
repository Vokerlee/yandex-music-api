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
| [albums/metadata/main.c](albums/metadata/main.c) | [albums/metadata/main.cpp](albums/metadata/main.cpp) | Fetch album metadata for comma-separated IDs | `ALBUM_IDS` |
| [albums/tracks/main.c](albums/tracks/main.c) | [albums/tracks/main.cpp](albums/tracks/main.cpp) | Read all album tracks grouped by disc | `ALBUM_ID` |
| [account/profile/main.c](account/profile/main.c) | [account/profile/main.cpp](account/profile/main.cpp) | Read the current account ID and login | `` |
| [account/liked_tracks/main.c](account/liked_tracks/main.c) | [account/liked_tracks/main.cpp](account/liked_tracks/main.cpp) | List liked track IDs and album IDs | `USER_ID` |
| [account/liked_artists/main.c](account/liked_artists/main.c) | [account/liked_artists/main.cpp](account/liked_artists/main.cpp) | List liked artists | `USER_ID` |
| [account/liked_albums/main.c](account/liked_albums/main.c) | [account/liked_albums/main.cpp](account/liked_albums/main.cpp) | List liked albums | `USER_ID` |
| [account/liked_playlists/main.c](account/liked_playlists/main.c) | [account/liked_playlists/main.cpp](account/liked_playlists/main.cpp) | List liked playlists | `USER_ID` |
| [account/set_like/main.c](account/set_like/main.c) | [account/set_like/main.cpp](account/set_like/main.cpp) | Like or unlike comma-separated tracks | `USER_ID TRACK_IDS like_or_unlike` |
| [playlists/list/main.c](playlists/list/main.c) | [playlists/list/main.cpp](playlists/list/main.cpp) | List an account's playlist IDs and titles | `USER_ID` |
| [playlists/tracks/main.c](playlists/tracks/main.c) | [playlists/tracks/main.cpp](playlists/tracks/main.cpp) | Read the tracks in a playlist | `USER_ID KIND` |
| [playlists/recommendations/main.c](playlists/recommendations/main.c) | [playlists/recommendations/main.cpp](playlists/recommendations/main.cpp) | Get recommendations for a playlist | `USER_ID KIND` |
| [playlists/create/main.c](playlists/create/main.c) | [playlists/create/main.cpp](playlists/create/main.cpp) | Create a private playlist | `USER_ID TITLE` |
| [playlists/delete/main.c](playlists/delete/main.c) | [playlists/delete/main.cpp](playlists/delete/main.cpp) | Delete the specified playlist | `USER_ID KIND` |
| [recommendations/feed/main.c](recommendations/feed/main.c) | [recommendations/feed/main.cpp](recommendations/feed/main.cpp) | List the account's generated discovery playlists | `` |
| [recommendations/personal/main.c](recommendations/personal/main.c) | [recommendations/personal/main.cpp](recommendations/personal/main.cpp) | Read a personal playlist such as daily or neverHeard | `PLAYLIST_ID` |
| [wave/start/main.c](wave/start/main.c) | [wave/start/main.cpp](wave/start/main.cpp) | Start My Wave or a station and print its session and tracks | `SEED` |
| [wave/next/main.c](wave/next/main.c) | [wave/next/main.cpp](wave/next/main.cpp) | Request more tracks without repeating the received IDs | `SESSION_ID RECEIVED_TRACK_IDS` |
| [wave/feedback/main.c](wave/feedback/main.c) | [wave/feedback/main.cpp](wave/feedback/main.cpp) | Report the start of track playback to the session | `SESSION_ID TRACK_ID BATCH_ID` |
| [wave/settings/main.c](wave/settings/main.c) | [wave/settings/main.cpp](wave/settings/main.cpp) | List available discovery, mood and language options and their seeds | `` |
| [wave/stations/main.c](wave/stations/main.c) | [wave/stations/main.cpp](wave/stations/main.cpp) | List genre, mood and activity station names and usable IDs | `` |
| [wave/discovery/main.c](wave/discovery/main.c) | [wave/discovery/main.cpp](wave/discovery/main.cpp) | Start unfamiliar discovery | `` |
| [wave/mood/main.c](wave/mood/main.c) | [wave/mood/main.cpp](wave/mood/main.cpp) | Start an available mood such as active or calm | `CHOICE` |
| [wave/artist/main.c](wave/artist/main.c) | [wave/artist/main.cpp](wave/artist/main.cpp) | Start radio similar to an artist | `ARTIST_ID` |
| [wave/genre/main.c](wave/genre/main.c) | [wave/genre/main.cpp](wave/genre/main.cpp) | Find a genre such as rap by its returned name or tag, then start its wave | `GENRE_NAME_OR_TAG` |
| [wave/custom/main.c](wave/custom/main.c) | [wave/custom/main.cpp](wave/custom/main.cpp) | Combine a genre or artist with discovery, mood or language settings | `BASE_SEED SETTING_SEED ANOTHER_SETTING_SEED` |

| [downloads/formats/main.c](downloads/formats/main.c) | [downloads/formats/main.cpp](downloads/formats/main.cpp) | List available legacy codecs and bitrates | `TRACK_ID` |

| [downloads/file_info/main.c](downloads/file_info/main.c) | [downloads/file_info/main.cpp](downloads/file_info/main.cpp) | Resolve raw audio metadata and the server-provided URL | `TRACK_ID` |

| [downloads/track/main.c](downloads/track/main.c) | [downloads/track/main.cpp](downloads/track/main.cpp) | Save a high-quality MP3 copy of a track | `TRACK_ID FILENAME` |

| [downloads/file/main.c](downloads/file/main.c) | [downloads/file/main.cpp](downloads/file/main.cpp) | Save a file from its HTTPS URL without forwarding an account token | `HTTPS_URL FILENAME` |

| [requests/generic/main.c](requests/generic/main.c) | [requests/generic/main.cpp](requests/generic/main.cpp) | Call an API route and inspect HTTP/request metadata | `` |

| [requests/polling/main.c](requests/polling/main.c) | [requests/polling/main.cpp](requests/polling/main.cpp) | Drive a request in an event loop with a 30-second deadline | `QUERY` |

| [requests/completion/main.c](requests/completion/main.c) | [requests/completion/main.cpp](requests/completion/main.cpp) | Receive completion on the thread driving the client | `QUERY` |

## Customize My Wave

List the service's current labels and seeds first:

```sh
./build/ym_example_cpp_wave_settings
./build/ym_example_cpp_wave_stations
```

Use the same arguments with the neighboring C program by changing `cpp` to `c` in the executable name.

```sh
# Unfamiliar tracks
./build/ym_example_cpp_wave_discovery

# Energetic/explosive or calm/chillout mood: select the returned mood value
./build/ym_example_cpp_wave_mood active
./build/ym_example_cpp_wave_mood calm

# Rap, or another genre's returned name/tag
./build/ym_example_cpp_wave_genre rap

# Music similar to an artist
./build/ym_example_cpp_wave_artist ARTIST_ID

# Combine a station and two settings printed by wave/settings
./build/ym_example_cpp_wave_custom user:onyourwave settingDiversity:discover settingMoodEnergy:active
```

Available labels and choices depend on the account and service. The mood and discovery programs resolve the chosen value through `serializedSeed`; the genre program uses the returned station ID.
