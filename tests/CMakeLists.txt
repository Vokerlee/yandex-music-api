add_executable(client_test test_client.cpp)
target_link_libraries(client_test PRIVATE yandex_music::cpp)
set_target_properties(client_test PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
add_test(NAME core.client COMMAND client_test)
