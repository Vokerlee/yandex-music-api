cmake_minimum_required(VERSION 3.28)

project(yandex_music VERSION 0.1.0 DESCRIPTION "Unofficial Yandex Music C/C++ SDK" LANGUAGES C CXX)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "Use an out-of-source build: cmake -S . -B build")
endif()

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(YM_BUILD_TESTS "Build SDK tests" ${PROJECT_IS_TOP_LEVEL})
option(YM_INSTALL "Install SDK headers, libraries and CMake targets" ${PROJECT_IS_TOP_LEVEL})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CheckCXXSourceCompiles)
function(ym_check_standard_library)
    set(CMAKE_CXX_STANDARD 23)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    check_cxx_source_compiles(
        "#include <expected>\nint main() { std::expected<int, int> value(1); return *value - 1; }"
        YM_HAS_STD_EXPECTED
    )
    if(NOT YM_HAS_STD_EXPECTED)
        message(
            FATAL_ERROR
            "The selected C++23 compiler/standard library must provide std::expected (for example GCC 13+)."
        )
    endif()
endfunction()
ym_check_standard_library()

configure_file(cmake/version_config.hpp.in generated/version_config.hpp @ONLY)
include(cmake/dependencies.cmake)
include(cmake/build.cmake)

add_library(yandex_music_c src/c/version.cpp)
ym_build_controls(yandex_music_c)
set(YM_CORE_SOURCES src/core/client.cpp src/core/error.cpp src/core/result.cpp src/core/engine.cpp)
target_sources(yandex_music_c PRIVATE ${YM_CORE_SOURCES})
target_include_directories(yandex_music_c PRIVATE src)
add_library(yandex_music::c ALIAS yandex_music_c)
set_target_properties(
    yandex_music_c
    PROPERTIES CXX_EXTENSIONS OFF CXX_VISIBILITY_PRESET hidden VISIBILITY_INLINES_HIDDEN ON
)
target_compile_features(yandex_music_c PRIVATE cxx_std_23)
target_compile_definitions(yandex_music_c PRIVATE YM_BUILDING_LIBRARY)
if(NOT BUILD_SHARED_LIBS)
    target_compile_definitions(yandex_music_c PUBLIC YM_STATIC)
endif()
target_include_directories(
    yandex_music_c
    PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/c>"
    PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/generated"
)
target_link_libraries(yandex_music_c PRIVATE "$<BUILD_INTERFACE:nlohmann_json::nlohmann_json>")

add_library(yandex_music_cpp INTERFACE)
add_library(yandex_music::cpp ALIAS yandex_music_cpp)
target_compile_features(yandex_music_cpp INTERFACE cxx_std_23)
target_link_libraries(yandex_music_cpp INTERFACE yandex_music_c)
target_include_directories(
    yandex_music_cpp
    INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/cpp>"
)

target_sources(
    yandex_music_c
    PUBLIC
        FILE_SET HEADERS
            BASE_DIRS src/c
            FILES src/c/yandex_music/yandex_music.h src/c/yandex_music/export.h
)

target_sources(
    yandex_music_c
    PUBLIC
        FILE_SET HEADERS
            FILES src/c/yandex_music/types.h src/c/yandex_music/error.h src/c/yandex_music/client.h
)
target_sources(
    yandex_music_cpp
    INTERFACE
        FILE_SET HEADERS
            BASE_DIRS src/cpp
            FILES src/cpp/yandex_music/error.hpp src/cpp/yandex_music/client.hpp
)
target_sources(
    yandex_music_cpp
    INTERFACE FILE_SET HEADERS BASE_DIRS src/cpp FILES src/cpp/yandex_music/yandex_music.hpp
)
set_target_properties(
    yandex_music_c
    PROPERTIES EXPORT_NAME c VERSION ${PROJECT_VERSION} SOVERSION 0 POSITION_INDEPENDENT_CODE ON
)
set_target_properties(yandex_music_cpp PROPERTIES EXPORT_NAME cpp)
target_sources(yandex_music_c PUBLIC FILE_SET HEADERS FILES src/c/yandex_music/request.h)
target_sources(yandex_music_cpp INTERFACE FILE_SET HEADERS FILES src/cpp/yandex_music/request.hpp)
target_sources(yandex_music_c PUBLIC FILE_SET HEADERS FILES src/c/yandex_music/result.h)
target_sources(
    yandex_music_cpp
    INTERFACE FILE_SET HEADERS FILES src/cpp/yandex_music/result.hpp src/cpp/yandex_music/ids.hpp
)

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
                "${CMAKE_COMMAND}" -DYM_SOURCE=${CMAKE_CURRENT_SOURCE_DIR}
                -DYM_BINARY=${CMAKE_CURRENT_BINARY_DIR}/consumer-source
                -DYM_SHARED=${BUILD_SHARED_LIBS} -DYM_JSON_SOURCE=${YM_JSON_SOURCE_DIR}
                "-DYM_PREFIX_PATH=${CMAKE_PREFIX_PATH}" -DYM_MODE=${YM_DEPENDENCY_MODE} -P
                ${CMAKE_CURRENT_SOURCE_DIR}/tests/consumer.cmake
        )
        add_test(
            NAME consumers.installed
            COMMAND
                "${CMAKE_COMMAND}" -DYM_SOURCE=${CMAKE_CURRENT_SOURCE_DIR}
                -DYM_BINARY=${CMAKE_CURRENT_BINARY_DIR}/consumer-installed
                -DYM_SHARED=${BUILD_SHARED_LIBS} -DYM_JSON_SOURCE=${YM_JSON_SOURCE_DIR}
                "-DYM_PREFIX_PATH=${CMAKE_PREFIX_PATH}" -DYM_MODE=${YM_DEPENDENCY_MODE} -P
                ${CMAKE_CURRENT_SOURCE_DIR}/tests/package.cmake
        )
        if(YM_JSON_SOURCE_DIR)
            add_test(
                NAME dependencies.modes
                COMMAND
                    "${CMAKE_COMMAND}" -DYM_SOURCE=${CMAKE_CURRENT_SOURCE_DIR}
                    -DYM_BINARY=${CMAKE_CURRENT_BINARY_DIR}/dependency-modes
                    -DYM_JSON_SOURCE=${YM_JSON_SOURCE_DIR} "-DYM_PREFIX_PATH=${CMAKE_PREFIX_PATH}"
                    -DYM_MODE=${YM_DEPENDENCY_MODE} -P
                    ${CMAKE_CURRENT_SOURCE_DIR}/tests/dependencies.cmake
            )
        endif()
    endif()
endif()
