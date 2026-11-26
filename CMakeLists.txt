cmake_minimum_required(VERSION 3.28)

project(yandex_music VERSION 0.1.0 DESCRIPTION "Unofficial Yandex Music C/C++ SDK" LANGUAGES C CXX)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "Use an out-of-source build: cmake -S . -B build")
endif()

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(YM_BUILD_TESTS "Build SDK tests" ${PROJECT_IS_TOP_LEVEL})
option(YM_INSTALL "Install SDK headers, libraries and CMake targets" ${PROJECT_IS_TOP_LEVEL})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

configure_file(cmake/version_config.hpp.in generated/version_config.hpp @ONLY)

add_library(yandex_music_c src/c/version.cpp)
add_library(yandex_music::c ALIAS yandex_music_c)
set_target_properties(yandex_music_c PROPERTIES CXX_EXTENSIONS OFF CXX_VISIBILITY_PRESET hidden VISIBILITY_INLINES_HIDDEN ON)
target_compile_features(yandex_music_c PRIVATE cxx_std_23)
target_compile_definitions(yandex_music_c PRIVATE YM_BUILDING_LIBRARY)
if(NOT BUILD_SHARED_LIBS)
    target_compile_definitions(yandex_music_c PUBLIC YM_STATIC)
endif()
target_include_directories(yandex_music_c PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/c>" PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/generated")

add_library(yandex_music_cpp INTERFACE)
add_library(yandex_music::cpp ALIAS yandex_music_cpp)
target_compile_features(yandex_music_cpp INTERFACE cxx_std_23)
target_link_libraries(yandex_music_cpp INTERFACE yandex_music_c)
target_include_directories(yandex_music_cpp INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/cpp>")

target_sources(yandex_music_c PUBLIC FILE_SET HEADERS BASE_DIRS src/c FILES src/c/yandex_music/yandex_music.h src/c/yandex_music/export.h)
target_sources(yandex_music_cpp INTERFACE FILE_SET HEADERS BASE_DIRS src/cpp FILES src/cpp/yandex_music/yandex_music.hpp)
set_target_properties(yandex_music_c PROPERTIES EXPORT_NAME c VERSION ${PROJECT_VERSION} SOVERSION 0 POSITION_INDEPENDENT_CODE ON)
set_target_properties(yandex_music_cpp PROPERTIES EXPORT_NAME cpp)

if(YM_INSTALL)
    include(cmake/install.cmake)
endif()

if(PROJECT_IS_TOP_LEVEL)
    include(CTest)
    if(BUILD_TESTING AND YM_BUILD_TESTS)
        add_test(NAME consumers.source COMMAND "${CMAKE_COMMAND}" -DYM_SOURCE=${CMAKE_CURRENT_SOURCE_DIR} -DYM_BINARY=${CMAKE_CURRENT_BINARY_DIR}/consumer-source -DYM_SHARED=${BUILD_SHARED_LIBS} -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/consumer.cmake)
        add_test(NAME consumers.installed COMMAND "${CMAKE_COMMAND}" -DYM_SOURCE=${CMAKE_CURRENT_SOURCE_DIR} -DYM_BINARY=${CMAKE_CURRENT_BINARY_DIR}/consumer-installed -DYM_SHARED=${BUILD_SHARED_LIBS} -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/package.cmake)
    endif()
endif()
