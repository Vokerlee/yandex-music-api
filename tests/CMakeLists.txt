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

set_tests_properties(
    c.version
    PROPERTIES ENVIRONMENT_MODIFICATION "PATH=path_list_prepend:$<TARGET_FILE_DIR:yandex_music::c>"
)
