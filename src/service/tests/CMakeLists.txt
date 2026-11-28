ym_test(api.search search.cpp ym_test_core)
ym_test(api.tracks tracks.cpp ym_test_core)
ym_test(api.artists artists.cpp ym_test_core)
ym_test(api.albums albums.cpp ym_test_core)
ym_test(api.account account.cpp ym_test_core)
ym_test(api.playlists playlists.cpp ym_test_core)
ym_test(api.playlist_editing playlist_editing.cpp ym_test_core)
ym_test(api.recommendations recommendations.cpp ym_test_core)
ym_test(api.wave wave.cpp ym_test_core)
ym_test(api.download_info download_info.cpp ym_test_core)

ym_test_target(api_downloads downloads.cpp ym_test_core)
add_test(
    NAME api.downloads
    COMMAND
        "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/download_fixture.py"
        $<TARGET_FILE:api_downloads>
)

ym_test(api.wave_seeds wave_seeds.cpp ym_test_core)
ym_test(api.wave_choices wave_choices.cpp ym_test_core)
