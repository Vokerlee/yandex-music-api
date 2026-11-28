cmake_minimum_required(VERSION 3.28)

project(yandex_music VERSION 0.1.0 LANGUAGES C CXX)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "Use an out-of-source build: cmake -S . -B build")
endif()

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(YM_BUILD_TESTS "Build SDK tests" ${PROJECT_IS_TOP_LEVEL})

configure_file(cmake/version_config.hpp.in generated/version_config.hpp @ONLY)

include(cmake/dependencies.cmake)

option(YM_INSTALL "Install SDK headers, libraries and CMake targets" ${PROJECT_IS_TOP_LEVEL})

include(cmake/curl.cmake)

ym_resolve_curl()

FetchContent_GetProperties(ym_curl SOURCE_DIR YM_CURL_SOURCE_DIR)

include(cmake/build.cmake)

set(YM_CORE_SOURCES
    src/core/error.cpp
    src/core/client.cpp
    src/core/api.cpp
    src/service/downloads.cpp
    src/service/download_file.cpp
    src/service/download_info.cpp
    src/service/file_signature.cpp
    src/service/wave.cpp
    src/service/wave_choices.cpp
    src/service/recommendations.cpp
    src/service/playlist_editing.cpp
    src/service/playlists.cpp
    src/service/account.cpp
    src/service/albums.cpp
    src/service/artists.cpp
    src/service/tracks.cpp
    src/service/search.cpp
    src/core/result.cpp
    src/core/request_options.cpp
    src/core/scheduler.cpp
    src/core/request_api.cpp
    src/core/wait.cpp
    src/protocol/percent.cpp
    src/protocol/fields.cpp
    src/protocol/encode.cpp
    src/protocol/json_write.cpp
    src/protocol/json_read.cpp
    src/protocol/envelope.cpp
    src/protocol/response_error.cpp
    src/c/response.cpp
    src/protocol/response.cpp
    src/transport/curl_runtime.cpp
    src/transport/response_sink.cpp
    src/transport/header_sink.cpp
    src/transport/curl_transfer.cpp
    src/transport/request_mapping.cpp
    src/transport/curl_multi.cpp
    src/transport/curl_adapter.cpp
)

add_library(yandex_music_c src/c/version.cpp ${YM_CORE_SOURCES})
add_library(yandex_music::c ALIAS yandex_music_c)

if(WIN32)
    set(YM_CRYPTO_LIBRARY bcrypt)
else()
    find_package(OpenSSL REQUIRED COMPONENTS Crypto)
    set(YM_CRYPTO_LIBRARY OpenSSL::Crypto)
endif()

target_link_libraries(yandex_music_c PRIVATE CURL::libcurl ${YM_CRYPTO_LIBRARY})

target_compile_features(yandex_music_c PRIVATE cxx_std_23)

set_target_properties(
    yandex_music_c
    PROPERTIES
        CXX_EXTENSIONS OFF
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON
        POSITION_INDEPENDENT_CODE ON
)

target_compile_definitions(yandex_music_c PRIVATE YM_BUILDING_LIBRARY)

if(NOT BUILD_SHARED_LIBS)
    target_compile_definitions(yandex_music_c PUBLIC YM_STATIC)
endif()

target_include_directories(yandex_music_c PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/generated" src)

target_sources(
    yandex_music_c
    PUBLIC
        FILE_SET HEADERS
            BASE_DIRS src/c
            FILES
                src/c/yandex_music/yandex_music.h
                src/c/yandex_music/export.h
                src/c/yandex_music/types.h
                src/c/yandex_music/error.h
                src/c/yandex_music/client.h
                src/c/yandex_music/api.h
                src/c/yandex_music/download.h
                src/c/yandex_music/wave.h
                src/c/yandex_music/recommendations.h
                src/c/yandex_music/playlists.h
                src/c/yandex_music/account.h
                src/c/yandex_music/catalogue.h
                src/c/yandex_music/search.h
                src/c/yandex_music/result.h
                src/c/yandex_music/request.h
                src/c/yandex_music/response.h
)

target_link_libraries(yandex_music_c PRIVATE "$<BUILD_INTERFACE:nlohmann_json::nlohmann_json>")

ym_build_controls(yandex_music_c)

add_library(yandex_music_cpp INTERFACE)
add_library(yandex_music::cpp ALIAS yandex_music_cpp)

target_compile_features(yandex_music_cpp INTERFACE cxx_std_23)

target_link_libraries(yandex_music_cpp INTERFACE yandex_music_c)

target_sources(
    yandex_music_cpp
    INTERFACE
        FILE_SET HEADERS
            BASE_DIRS src/cpp
            FILES
                src/cpp/yandex_music/yandex_music.hpp
                src/cpp/yandex_music/error.hpp
                src/cpp/yandex_music/client.hpp
                src/cpp/yandex_music/download.hpp
                src/cpp/yandex_music/result.hpp
                src/cpp/yandex_music/ids.hpp
                src/cpp/yandex_music/request.hpp
                src/cpp/yandex_music/response.hpp
)

option(YM_INSTALL "Install SDK headers, libraries and CMake targets" ${PROJECT_IS_TOP_LEVEL})

set_target_properties(
    yandex_music_c
    PROPERTIES EXPORT_NAME c VERSION ${PROJECT_VERSION} SOVERSION 0
)

set_target_properties(yandex_music_cpp PROPERTIES EXPORT_NAME cpp)

if(YM_INSTALL)
    include(cmake/install.cmake)
endif()

if(PROJECT_IS_TOP_LEVEL)
    include(CTest)

    if(BUILD_TESTING AND YM_BUILD_TESTS)
        add_subdirectory(tests)

        add_test(
            NAME consumers.source
            COMMAND
                "${CMAKE_COMMAND}" -DYM_CURL_SOURCE=${YM_CURL_SOURCE_DIR}
                -DYM_CURL_MODE=${YM_CURL_DEPENDENCY_MODE} -DYM_SOURCE=${CMAKE_CURRENT_SOURCE_DIR}
                -DYM_BINARY=${CMAKE_CURRENT_BINARY_DIR}/consumers.source
                -DYM_SHARED=${BUILD_SHARED_LIBS} -DYM_JSON_SOURCE=${YM_JSON_SOURCE_DIR}
                "-DYM_PREFIX_PATH=${CMAKE_PREFIX_PATH}" -DYM_MODE=${YM_DEPENDENCY_MODE} -P
                ${CMAKE_CURRENT_SOURCE_DIR}/tests/consumer.cmake
        )
        add_test(
            NAME consumers.installed
            COMMAND
                "${CMAKE_COMMAND}" -DYM_CURL_SOURCE=${YM_CURL_SOURCE_DIR}
                -DYM_CURL_MODE=${YM_CURL_DEPENDENCY_MODE} -DYM_SOURCE=${CMAKE_CURRENT_SOURCE_DIR}
                -DYM_BINARY=${CMAKE_CURRENT_BINARY_DIR}/consumers.installed
                -DYM_SHARED=${BUILD_SHARED_LIBS} -DYM_JSON_SOURCE=${YM_JSON_SOURCE_DIR}
                "-DYM_PREFIX_PATH=${CMAKE_PREFIX_PATH}" -DYM_MODE=${YM_DEPENDENCY_MODE} -P
                ${CMAKE_CURRENT_SOURCE_DIR}/tests/package.cmake
        )
        add_test(
            NAME dependencies.modes
            COMMAND
                "${CMAKE_COMMAND}" -DYM_CURL_SOURCE=${YM_CURL_SOURCE_DIR}
                -DYM_CURL_MODE=${YM_CURL_DEPENDENCY_MODE} -DYM_SOURCE=${CMAKE_CURRENT_SOURCE_DIR}
                -DYM_BINARY=${CMAKE_CURRENT_BINARY_DIR}/dependencies.modes
                -DYM_SHARED=${BUILD_SHARED_LIBS} -DYM_JSON_SOURCE=${YM_JSON_SOURCE_DIR}
                "-DYM_PREFIX_PATH=${CMAKE_PREFIX_PATH}" -DYM_MODE=${YM_DEPENDENCY_MODE} -P
                ${CMAKE_CURRENT_SOURCE_DIR}/tests/dependencies.cmake
        )
        add_test(
            NAME dependencies.curl
            COMMAND
                "${CMAKE_COMMAND}" -DYM_CURL_SOURCE=${YM_CURL_SOURCE_DIR}
                -DYM_CURL_MODE=${YM_CURL_DEPENDENCY_MODE} -DYM_SOURCE=${CMAKE_CURRENT_SOURCE_DIR}
                -DYM_BINARY=${CMAKE_CURRENT_BINARY_DIR}/dependencies.curl
                -DYM_SHARED=${BUILD_SHARED_LIBS} -DYM_JSON_SOURCE=${YM_JSON_SOURCE_DIR}
                "-DYM_PREFIX_PATH=${CMAKE_PREFIX_PATH}" -DYM_MODE=${YM_DEPENDENCY_MODE} -P
                ${CMAKE_CURRENT_SOURCE_DIR}/tests/curl-dependency.cmake
        )
    endif()
endif()
