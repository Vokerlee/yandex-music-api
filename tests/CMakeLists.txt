add_executable(client_test test_client.cpp)
target_link_libraries(client_test PRIVATE yandex_music::cpp)
add_test(NAME core.client COMMAND client_test)

add_executable(result_test test_result.cpp)
target_include_directories(result_test PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(result_test PRIVATE yandex_music::cpp)
add_test(NAME core.result COMMAND result_test)
