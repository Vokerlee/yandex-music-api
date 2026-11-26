cmake_minimum_required(VERSION 3.28)

project(yandex_music VERSION 0.1.0 LANGUAGES C CXX)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "Use an out-of-source build: cmake -S . -B build")
endif()

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(YM_BUILD_TESTS "Build SDK tests" ${PROJECT_IS_TOP_LEVEL})

configure_file(cmake/version_config.hpp.in generated/version_config.hpp @ONLY)

add_library(yandex_music_c src/c/version.cpp)
add_library(yandex_music::c ALIAS yandex_music_c)

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
            FILES src/c/yandex_music/yandex_music.h src/c/yandex_music/export.h
)

add_library(yandex_music_cpp INTERFACE)
add_library(yandex_music::cpp ALIAS yandex_music_cpp)

target_compile_features(yandex_music_cpp INTERFACE cxx_std_23)

target_link_libraries(yandex_music_cpp INTERFACE yandex_music_c)

target_sources(
    yandex_music_cpp
    INTERFACE FILE_SET HEADERS BASE_DIRS src/cpp FILES src/cpp/yandex_music/yandex_music.hpp
)

if(PROJECT_IS_TOP_LEVEL)
    include(CTest)

    if(BUILD_TESTING AND YM_BUILD_TESTS)
        add_subdirectory(tests)
    endif()
endif()
