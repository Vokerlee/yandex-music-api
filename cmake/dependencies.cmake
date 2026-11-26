include(FetchContent)

set(YM_DEPENDENCY_MODE AUTO CACHE STRING "Dependency resolution: AUTO, SYSTEM or FETCH")
set_property(CACHE YM_DEPENDENCY_MODE PROPERTY STRINGS AUTO SYSTEM FETCH)
if(NOT YM_DEPENDENCY_MODE MATCHES "^(AUTO|SYSTEM|FETCH)$")
    message(FATAL_ERROR "YM_DEPENDENCY_MODE must be AUTO, SYSTEM or FETCH")
endif()

function(ym_resolve_json)
    if(TARGET nlohmann_json::nlohmann_json)
        return()
    endif()
    if(NOT YM_DEPENDENCY_MODE STREQUAL "FETCH")
        find_package(nlohmann_json 3.12 QUIET CONFIG)
    endif()
    if(TARGET nlohmann_json::nlohmann_json)
        return()
    endif()
    if(YM_DEPENDENCY_MODE STREQUAL "SYSTEM")
        message(
            FATAL_ERROR
            "Install nlohmann_json >= 3.12 or set CMAKE_PREFIX_PATH; SYSTEM mode never downloads dependencies"
        )
    endif()

    # Normal variables are scoped here: dependency options must not change parent caches.
    set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
    set(JSON_BuildTests OFF)
    set(JSON_Install OFF)
    set(JSON_MultipleHeaders ON)
    FetchContent_Declare(
        nlohmann_json
        URL https://codeload.github.com/nlohmann/json/tar.gz/refs/tags/v3.12.0
        URL_HASH SHA256=4b92eb0c06d10683f7447ce9406cb97cd4b453be18d7279320f7b2f025c10187
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    FetchContent_MakeAvailable(nlohmann_json)
endfunction()

ym_resolve_json()
FetchContent_GetProperties(nlohmann_json SOURCE_DIR YM_JSON_SOURCE_DIR)
