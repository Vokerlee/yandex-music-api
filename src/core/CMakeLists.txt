ym_core_sources(
    api.cpp
    client.cpp
    error.cpp
    request_api.cpp
    request_options.cpp
    result.cpp
    scheduler.cpp
    wait.cpp
)

if(TARGET ym_test_core)
    add_subdirectory(tests)
endif()
