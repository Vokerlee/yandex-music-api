add_executable(c_version consumers/main.c)
set_target_properties(
    c_version
    PROPERTIES
        C_STANDARD 11
        C_STANDARD_REQUIRED ON
        C_EXTENSIONS OFF
        RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
)
target_link_libraries(c_version PRIVATE yandex_music::c)
add_test(NAME c.version COMMAND c_version)

add_executable(cpp_version consumers/main.cpp)
set_target_properties(cpp_version PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
target_link_libraries(cpp_version PRIVATE yandex_music::cpp)
add_test(NAME cpp.version COMMAND cpp_version)

add_executable(abi_errors abi.errors.cpp)
target_include_directories(abi_errors PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(abi_errors PRIVATE yandex_music::cpp)
set_target_properties(abi_errors PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
add_test(NAME abi.errors COMMAND abi_errors)

add_executable(cpp_errors cpp.errors.cpp)
target_include_directories(cpp_errors PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(cpp_errors PRIVATE yandex_music::cpp)
set_target_properties(cpp_errors PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
add_test(NAME cpp.errors COMMAND cpp_errors)

add_executable(abi_client_options abi.client_options.cpp)
target_include_directories(abi_client_options PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(abi_client_options PRIVATE yandex_music::cpp)
set_target_properties(
    abi_client_options
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
)
add_test(NAME abi.client_options COMMAND abi_client_options)

add_executable(abi_client_owner abi.client_owner.cpp)
target_include_directories(abi_client_owner PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(abi_client_owner PRIVATE yandex_music::cpp)
set_target_properties(abi_client_owner PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
add_test(NAME abi.client_owner COMMAND abi_client_owner)

set_tests_properties(
    c.version
    cpp.version
    abi.errors
    cpp.errors
    abi.client_options
    abi.client_owner
    PROPERTIES ENVIRONMENT_MODIFICATION "PATH=path_list_prepend:$<TARGET_FILE_DIR:yandex_music::c>"
)
