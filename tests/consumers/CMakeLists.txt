cmake_minimum_required(VERSION 3.28)
project(yandex_music_consumer LANGUAGES C CXX)

add_subdirectory("${YM_SOURCE}" sdk)
add_executable(c_consumer main.c)
set_target_properties(c_consumer PROPERTIES C_STANDARD 11 C_STANDARD_REQUIRED ON C_EXTENSIONS OFF)
target_link_libraries(c_consumer PRIVATE yandex_music::c)
add_executable(cpp_consumer main.cpp)
target_link_libraries(cpp_consumer PRIVATE yandex_music::cpp)

enable_testing()
add_test(NAME consumer.c COMMAND c_consumer)
add_test(NAME consumer.cpp COMMAND cpp_consumer)

set_tests_properties(consumer.c consumer.cpp PROPERTIES
    ENVIRONMENT_MODIFICATION "PATH=path_list_prepend:$<TARGET_FILE_DIR:yandex_music::c>")
