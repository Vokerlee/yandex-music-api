add_library(yandex_music_c version.cpp response.cpp)
add_library(yandex_music::c ALIAS yandex_music_c)
target_compile_features(yandex_music_c PRIVATE cxx_std_23)
target_compile_definitions(yandex_music_c PRIVATE YM_BUILDING_LIBRARY)
if(NOT BUILD_SHARED_LIBS)
    target_compile_definitions(yandex_music_c PUBLIC YM_STATIC)
endif()
set_target_properties(
    yandex_music_c
    PROPERTIES
        CXX_EXTENSIONS OFF
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON
        POSITION_INDEPENDENT_CODE ON
        EXPORT_NAME c
        VERSION ${PROJECT_VERSION}
        SOVERSION 0
        RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
        LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
        ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
)
target_include_directories(
    yandex_music_c
    PRIVATE "${PROJECT_BINARY_DIR}/generated" "${PROJECT_SOURCE_DIR}/src"
)
target_link_libraries(
    yandex_music_c
    PRIVATE CURL::libcurl ${YM_CRYPTO_LIBRARY} "$<BUILD_INTERFACE:nlohmann_json::nlohmann_json>"
)
target_sources(
    yandex_music_c
    PUBLIC
        FILE_SET HEADERS
            BASE_DIRS "${PROJECT_SOURCE_DIR}/src"
            FILES
                account.h
                api.h
                catalogue.h
                client.h
                download.h
                error.h
                export.h
                playlists.h
                recommendations.h
                request.h
                response.h
                result.h
                search.h
                types.h
                wave.h
                yandex_music.h
)
ym_build_controls(yandex_music_c)

if(TARGET ym_test_core)
    target_sources(ym_test_core PRIVATE response.cpp)
    add_subdirectory(tests)
endif()
