cmake_minimum_required(VERSION 3.28)

project(yandex_music VERSION 0.1.0 LANGUAGES C CXX)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "Use an out-of-source build: cmake -S . -B build")
endif()

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(YM_BUILD_TESTS "Build SDK tests" ${PROJECT_IS_TOP_LEVEL})
option(YM_BUILD_EXAMPLES "Build standalone C and C++ examples" OFF)
option(YM_INSTALL "Install SDK headers, libraries and CMake targets" ${PROJECT_IS_TOP_LEVEL})

configure_file(cmake/version_config.hpp.in generated/version_config.hpp @ONLY)

include(cmake/dependencies.cmake)
include(cmake/curl.cmake)
ym_resolve_curl()
FetchContent_GetProperties(ym_curl SOURCE_DIR YM_CURL_SOURCE_DIR)

if(WIN32)
    set(YM_CRYPTO_LIBRARY bcrypt)
else()
    find_package(OpenSSL REQUIRED COMPONENTS Crypto)
    set(YM_CRYPTO_LIBRARY OpenSSL::Crypto)
endif()

include(cmake/build.cmake)
if(PROJECT_IS_TOP_LEVEL)
    include(CTest)
    if(BUILD_TESTING AND YM_BUILD_TESTS)
        include(cmake/tests.cmake)
    endif()
endif()

add_subdirectory(src)

if(YM_INSTALL)
    include(cmake/install.cmake)
endif()

if(TARGET ym_test_core)
    add_subdirectory(tests)
endif()

if(YM_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
