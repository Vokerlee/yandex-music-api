ym_core_sources(
    account.cpp
    albums.cpp
    artists.cpp
    download_file.cpp
    download_info.cpp
    downloads.cpp
    file_signature.cpp
    playlist_editing.cpp
    playlists.cpp
    recommendations.cpp
    search.cpp
    tracks.cpp
    wave.cpp
    wave_choices.cpp
)

if(TARGET ym_test_core)
    add_subdirectory(tests)
endif()
