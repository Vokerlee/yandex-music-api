ym_core_sources(
    curl_adapter.cpp
    curl_multi.cpp
    curl_runtime.cpp
    curl_transfer.cpp
    header_sink.cpp
    request_mapping.cpp
    response_sink.cpp
)

if(TARGET ym_test_core)
    add_subdirectory(tests)
endif()
