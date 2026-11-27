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

list(TRANSFORM YM_CORE_SOURCES PREPEND "${PROJECT_SOURCE_DIR}/" OUTPUT_VARIABLE private_sources)

add_library(ym_test_core STATIC ${private_sources})
target_include_directories(
    ym_test_core
    PUBLIC "${PROJECT_SOURCE_DIR}/src" "${PROJECT_SOURCE_DIR}/src/c" "${PROJECT_SOURCE_DIR}/src/cpp"
)
target_compile_features(ym_test_core PUBLIC cxx_std_23)
target_compile_definitions(ym_test_core PUBLIC YM_STATIC)
target_link_libraries(ym_test_core PRIVATE nlohmann_json::nlohmann_json)

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

add_executable(abi_option_prefixes abi.option_prefixes.cpp)
target_include_directories(abi_option_prefixes PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(abi_option_prefixes PRIVATE yandex_music::cpp)
set_target_properties(
    abi_option_prefixes
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
)
add_test(NAME abi.option_prefixes COMMAND abi_option_prefixes)

add_executable(cpp_client_owner cpp.client_owner.cpp)
target_include_directories(cpp_client_owner PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(cpp_client_owner PRIVATE yandex_music::cpp)
set_target_properties(cpp_client_owner PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
add_test(NAME cpp.client_owner COMMAND cpp_client_owner)

add_executable(abi_allocation_failure abi.allocation_failure.cpp)
target_include_directories(abi_allocation_failure PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(abi_allocation_failure PRIVATE yandex_music::cpp)
set_target_properties(
    abi_allocation_failure
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
)
add_test(NAME abi.allocation_failure COMMAND abi_allocation_failure)

add_executable(abi_result_owner abi.result_owner.cpp)
target_include_directories(abi_result_owner PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(abi_result_owner PRIVATE ym_test_core)
set_target_properties(abi_result_owner PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
add_test(NAME abi.result_owner COMMAND abi_result_owner)

add_executable(abi_scalar_views abi.scalar_views.cpp)
target_include_directories(abi_scalar_views PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(abi_scalar_views PRIVATE ym_test_core)
set_target_properties(abi_scalar_views PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
add_test(NAME abi.scalar_views COMMAND abi_scalar_views)

add_executable(abi_collection_views abi.collection_views.cpp)
target_include_directories(abi_collection_views PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(abi_collection_views PRIVATE ym_test_core)
set_target_properties(
    abi_collection_views
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
)
add_test(NAME abi.collection_views COMMAND abi_collection_views)

add_executable(cpp_document_owner cpp.document_owner.cpp)
target_include_directories(cpp_document_owner PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(cpp_document_owner PRIVATE ym_test_core)
set_target_properties(
    cpp_document_owner
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
)
add_test(NAME cpp.document_owner COMMAND cpp_document_owner)

add_executable(cpp_scalar_views cpp.scalar_views.cpp)
target_include_directories(cpp_scalar_views PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(cpp_scalar_views PRIVATE ym_test_core)
set_target_properties(cpp_scalar_views PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
add_test(NAME cpp.scalar_views COMMAND cpp_scalar_views)

add_executable(cpp_collection_views cpp.collection_views.cpp)
target_include_directories(cpp_collection_views PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(cpp_collection_views PRIVATE ym_test_core)
set_target_properties(
    cpp_collection_views
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
)
add_test(NAME cpp.collection_views COMMAND cpp_collection_views)

add_executable(domain_identities domain.identities.cpp)
target_include_directories(domain_identities PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(domain_identities PRIVATE yandex_music::cpp)
set_target_properties(domain_identities PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
add_test(NAME domain.identities COMMAND domain_identities)

add_executable(transport_contract transport.contract.cpp)
target_include_directories(transport_contract PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(transport_contract PRIVATE ym_test_core)
set_target_properties(
    transport_contract
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
)
add_test(NAME transport.contract COMMAND transport_contract)

add_executable(engine_scheduling engine.scheduling.cpp)
target_include_directories(engine_scheduling PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(engine_scheduling PRIVATE ym_test_core)
set_target_properties(engine_scheduling PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
add_test(NAME engine.scheduling COMMAND engine_scheduling)

set_tests_properties(
    c.version
    cpp.version
    abi.errors
    cpp.errors
    abi.client_options
    abi.client_owner
    abi.option_prefixes
    cpp.client_owner
    abi.allocation_failure
    abi.result_owner
    abi.scalar_views
    abi.collection_views
    cpp.document_owner
    cpp.scalar_views
    cpp.collection_views
    domain.identities
    transport.contract
    engine.scheduling
    PROPERTIES ENVIRONMENT_MODIFICATION "PATH=path_list_prepend:$<TARGET_FILE_DIR:yandex_music::c>"
)
