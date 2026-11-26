add_executable(client_test test_client.cpp)
target_link_libraries(client_test PRIVATE yandex_music::cpp)
set_target_properties(client_test PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
add_test(NAME core.client COMMAND client_test)

add_executable(result_test test_result.cpp)
target_include_directories(result_test PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(result_test PRIVATE yandex_music::cpp)
set_target_properties(result_test PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
add_test(NAME core.result COMMAND result_test)

# Private engine tests use the same sources with a deterministic adapter, without
# exporting a test transport or hidden C++ symbols from the installed library.
list(TRANSFORM YM_CORE_SOURCES PREPEND "${PROJECT_SOURCE_DIR}/" OUTPUT_VARIABLE test_sources)
add_library(ym_test_core STATIC ${test_sources})
target_include_directories(
    ym_test_core
    PUBLIC "${PROJECT_SOURCE_DIR}/src" "${PROJECT_SOURCE_DIR}/src/c" "${PROJECT_SOURCE_DIR}/src/cpp"
)
target_compile_features(ym_test_core PUBLIC cxx_std_23)
target_compile_definitions(ym_test_core PUBLIC YM_STATIC)
target_link_libraries(ym_test_core PRIVATE nlohmann_json::nlohmann_json)
add_executable(engine_test test_engine.cpp)
target_link_libraries(engine_test PRIVATE ym_test_core)
add_test(NAME core.engine COMMAND engine_test)
add_executable(encoding_test test_encoding.cpp)
target_link_libraries(encoding_test PRIVATE ym_test_core)
add_test(NAME protocol.encoding COMMAND encoding_test)
add_executable(response_test test_response.cpp)
target_link_libraries(response_test PRIVATE ym_test_core)
add_test(NAME protocol.response COMMAND response_test)
foreach(
    target
    client_test
    result_test
    ym_test_core
    engine_test
    encoding_test
    response_test
)
    ym_build_controls(${target})
endforeach()
