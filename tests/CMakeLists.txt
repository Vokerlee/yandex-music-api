add_executable(client_test test_client.cpp)
target_link_libraries(client_test PRIVATE yandex_music::cpp)
add_test(NAME core.client COMMAND client_test)
