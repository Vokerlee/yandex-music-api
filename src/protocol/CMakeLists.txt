ym_core_sources(
    encode.cpp
    envelope.cpp
    fields.cpp
    json_read.cpp
    json_write.cpp
    percent.cpp
    response.cpp
    response_error.cpp
)

if(TARGET ym_test_core)
    add_subdirectory(tests)
endif()
