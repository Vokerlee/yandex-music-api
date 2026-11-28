find_package(Python3 COMPONENTS Interpreter REQUIRED)

add_library(ym_test_core STATIC)
target_include_directories(
    ym_test_core
    PUBLIC "${PROJECT_SOURCE_DIR}/src" "${PROJECT_SOURCE_DIR}/tests"
)
target_compile_features(ym_test_core PUBLIC cxx_std_23)
target_compile_definitions(ym_test_core PUBLIC YM_STATIC)
target_link_libraries(
    ym_test_core
    PRIVATE nlohmann_json::nlohmann_json ${YM_CRYPTO_LIBRARY}
    PUBLIC CURL::libcurl
)
ym_build_controls(ym_test_core)

function(ym_test_target target source library)
    add_executable(${target} "${source}")
    target_include_directories(
        ${target}
        PRIVATE "${PROJECT_SOURCE_DIR}/src" "${PROJECT_SOURCE_DIR}/tests"
    )
    target_link_libraries(${target} PRIVATE ${library})
    set_target_properties(
        ${target}
        PROPERTIES
            C_STANDARD 11
            C_EXTENSIONS OFF
            CXX_EXTENSIONS OFF
            RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
    )
    ym_build_controls(${target})
endfunction()

function(ym_test name source library)
    string(REPLACE "." "_" target "${name}")
    ym_test_target(${target} "${source}" ${library})
    add_test(NAME ${name} COMMAND ${target})
    set_tests_properties(
        ${name}
        PROPERTIES
            ENVIRONMENT_MODIFICATION "PATH=path_list_prepend:$<TARGET_FILE_DIR:yandex_music::c>"
    )
endfunction()
