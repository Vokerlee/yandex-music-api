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
ym_build_controls(c_version)
ym_build_controls(cpp_version)

list(TRANSFORM YM_CORE_SOURCES PREPEND "${PROJECT_SOURCE_DIR}/" OUTPUT_VARIABLE private_sources)

add_library(ym_test_core STATIC ${private_sources})
target_include_directories(
    ym_test_core
    PUBLIC "${PROJECT_SOURCE_DIR}/src" "${PROJECT_SOURCE_DIR}/src/c" "${PROJECT_SOURCE_DIR}/src/cpp"
)
target_compile_features(ym_test_core PUBLIC cxx_std_23)
target_compile_definitions(ym_test_core PUBLIC YM_STATIC)
target_link_libraries(ym_test_core PRIVATE nlohmann_json::nlohmann_json PUBLIC CURL::libcurl)
ym_build_controls(ym_test_core)

add_executable(abi_errors abi.errors.cpp)
target_include_directories(abi_errors PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(abi_errors PRIVATE yandex_music::cpp)
set_target_properties(abi_errors PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
add_test(NAME abi.errors COMMAND abi_errors)
ym_build_controls(abi_errors)

add_executable(cpp_errors cpp.errors.cpp)
target_include_directories(cpp_errors PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(cpp_errors PRIVATE yandex_music::cpp)
set_target_properties(cpp_errors PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
add_test(NAME cpp.errors COMMAND cpp_errors)
ym_build_controls(cpp_errors)

add_executable(abi_client_options abi.client_options.cpp)
target_include_directories(abi_client_options PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(abi_client_options PRIVATE yandex_music::cpp)
set_target_properties(
    abi_client_options
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
)
add_test(NAME abi.client_options COMMAND abi_client_options)
ym_build_controls(abi_client_options)

add_executable(abi_client_owner abi.client_owner.cpp)
target_include_directories(abi_client_owner PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(abi_client_owner PRIVATE yandex_music::cpp)
set_target_properties(abi_client_owner PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
add_test(NAME abi.client_owner COMMAND abi_client_owner)
ym_build_controls(abi_client_owner)

add_executable(abi_option_prefixes abi.option_prefixes.cpp)
target_include_directories(abi_option_prefixes PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(abi_option_prefixes PRIVATE yandex_music::cpp)
set_target_properties(
    abi_option_prefixes
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
)
add_test(NAME abi.option_prefixes COMMAND abi_option_prefixes)
ym_build_controls(abi_option_prefixes)

add_executable(cpp_client_owner cpp.client_owner.cpp)
target_include_directories(cpp_client_owner PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(cpp_client_owner PRIVATE yandex_music::cpp)
set_target_properties(cpp_client_owner PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
add_test(NAME cpp.client_owner COMMAND cpp_client_owner)
ym_build_controls(cpp_client_owner)

add_executable(abi_allocation_failure abi.allocation_failure.cpp)
target_include_directories(abi_allocation_failure PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(abi_allocation_failure PRIVATE yandex_music::cpp)
set_target_properties(
    abi_allocation_failure
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
)
add_test(NAME abi.allocation_failure COMMAND abi_allocation_failure)
ym_build_controls(abi_allocation_failure)

add_executable(abi_result_owner abi.result_owner.cpp)
target_include_directories(abi_result_owner PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(abi_result_owner PRIVATE ym_test_core)
set_target_properties(abi_result_owner PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
add_test(NAME abi.result_owner COMMAND abi_result_owner)
ym_build_controls(abi_result_owner)

add_executable(abi_scalar_views abi.scalar_views.cpp)
target_include_directories(abi_scalar_views PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(abi_scalar_views PRIVATE ym_test_core)
set_target_properties(abi_scalar_views PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
add_test(NAME abi.scalar_views COMMAND abi_scalar_views)
ym_build_controls(abi_scalar_views)

add_executable(abi_collection_views abi.collection_views.cpp)
target_include_directories(abi_collection_views PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(abi_collection_views PRIVATE ym_test_core)
set_target_properties(
    abi_collection_views
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
)
add_test(NAME abi.collection_views COMMAND abi_collection_views)
ym_build_controls(abi_collection_views)

add_executable(cpp_document_owner cpp.document_owner.cpp)
target_include_directories(cpp_document_owner PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(cpp_document_owner PRIVATE ym_test_core)
set_target_properties(
    cpp_document_owner
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
)
add_test(NAME cpp.document_owner COMMAND cpp_document_owner)
ym_build_controls(cpp_document_owner)

add_executable(cpp_scalar_views cpp.scalar_views.cpp)
target_include_directories(cpp_scalar_views PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(cpp_scalar_views PRIVATE ym_test_core)
set_target_properties(cpp_scalar_views PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
add_test(NAME cpp.scalar_views COMMAND cpp_scalar_views)
ym_build_controls(cpp_scalar_views)

add_executable(cpp_collection_views cpp.collection_views.cpp)
target_include_directories(cpp_collection_views PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(cpp_collection_views PRIVATE ym_test_core)
set_target_properties(
    cpp_collection_views
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
)
add_test(NAME cpp.collection_views COMMAND cpp_collection_views)
ym_build_controls(cpp_collection_views)

add_executable(domain_identities domain.identities.cpp)
target_include_directories(domain_identities PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(domain_identities PRIVATE yandex_music::cpp)
set_target_properties(domain_identities PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
add_test(NAME domain.identities COMMAND domain_identities)
ym_build_controls(domain_identities)

add_executable(transport_contract transport.contract.cpp)
target_include_directories(transport_contract PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(transport_contract PRIVATE ym_test_core)
set_target_properties(
    transport_contract
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
)
add_test(NAME transport.contract COMMAND transport_contract)
ym_build_controls(transport_contract)

add_executable(engine_scheduling engine.scheduling.cpp)
target_include_directories(engine_scheduling PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(engine_scheduling PRIVATE ym_test_core)
set_target_properties(engine_scheduling PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
add_test(NAME engine.scheduling COMMAND engine_scheduling)
ym_build_controls(engine_scheduling)

add_executable(abi_request_owner abi.request_owner.cpp)
target_include_directories(abi_request_owner PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(abi_request_owner PRIVATE ym_test_core)
set_target_properties(abi_request_owner PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
add_test(NAME abi.request_owner COMMAND abi_request_owner)
ym_build_controls(abi_request_owner)

add_executable(engine_cancellation engine.cancellation.cpp)
target_include_directories(engine_cancellation PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(engine_cancellation PRIVATE ym_test_core)
set_target_properties(
    engine_cancellation
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
)
add_test(NAME engine.cancellation COMMAND engine_cancellation)
ym_build_controls(engine_cancellation)

add_executable(engine_limits engine.limits.cpp)
target_include_directories(engine_limits PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(engine_limits PRIVATE ym_test_core)
set_target_properties(engine_limits PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
add_test(NAME engine.limits COMMAND engine_limits)
ym_build_controls(engine_limits)

add_executable(engine_callbacks engine.callbacks.cpp)
target_include_directories(engine_callbacks PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(engine_callbacks PRIVATE ym_test_core)
set_target_properties(engine_callbacks PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
add_test(NAME engine.callbacks COMMAND engine_callbacks)
ym_build_controls(engine_callbacks)

add_executable(engine_callback_lifetimes engine.callback_lifetimes.cpp)
target_include_directories(engine_callback_lifetimes PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(engine_callback_lifetimes PRIVATE ym_test_core)
set_target_properties(
    engine_callback_lifetimes
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
)
add_test(NAME engine.callback_lifetimes COMMAND engine_callback_lifetimes)
ym_build_controls(engine_callback_lifetimes)

add_executable(cpp_request_owner cpp.request_owner.cpp)
target_include_directories(cpp_request_owner PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(cpp_request_owner PRIVATE ym_test_core)
set_target_properties(cpp_request_owner PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
add_test(NAME cpp.request_owner COMMAND cpp_request_owner)
ym_build_controls(cpp_request_owner)

add_executable(text_utf8 text.utf8.cpp)
target_include_directories(text_utf8 PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(text_utf8 PRIVATE ym_test_core)
set_target_properties(text_utf8 PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
add_test(NAME text.utf8 COMMAND text_utf8)
ym_build_controls(text_utf8)

add_executable(protocol_percent protocol.percent.cpp)
target_include_directories(protocol_percent PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(protocol_percent PRIVATE ym_test_core)
set_target_properties(protocol_percent PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
add_test(NAME protocol.percent COMMAND protocol_percent)
ym_build_controls(protocol_percent)

add_executable(protocol_fields protocol.fields.cpp)
target_include_directories(protocol_fields PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(protocol_fields PRIVATE ym_test_core)
set_target_properties(protocol_fields PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
add_test(NAME protocol.fields COMMAND protocol_fields)
ym_build_controls(protocol_fields)

add_executable(protocol_uri protocol.uri.cpp)
target_include_directories(protocol_uri PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(protocol_uri PRIVATE ym_test_core)
set_target_properties(protocol_uri PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
add_test(NAME protocol.uri COMMAND protocol_uri)
ym_build_controls(protocol_uri)

add_executable(protocol_form_body protocol.form_body.cpp)
target_include_directories(protocol_form_body PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(protocol_form_body PRIVATE ym_test_core)
set_target_properties(
    protocol_form_body
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
)
add_test(NAME protocol.form_body COMMAND protocol_form_body)
ym_build_controls(protocol_form_body)

add_executable(protocol_json_body protocol.json_body.cpp)
target_include_directories(protocol_json_body PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(protocol_json_body PRIVATE ym_test_core)
set_target_properties(
    protocol_json_body
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
)
add_test(NAME protocol.json_body COMMAND protocol_json_body)
ym_build_controls(protocol_json_body)

add_executable(protocol_headers protocol.headers.cpp)
target_include_directories(protocol_headers PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(protocol_headers PRIVATE ym_test_core)
set_target_properties(protocol_headers PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
add_test(NAME protocol.headers COMMAND protocol_headers)
ym_build_controls(protocol_headers)

add_executable(protocol_json_read protocol.json_read.cpp)
target_include_directories(protocol_json_read PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(protocol_json_read PRIVATE ym_test_core)
set_target_properties(
    protocol_json_read
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
)
add_test(NAME protocol.json_read COMMAND protocol_json_read)
ym_build_controls(protocol_json_read)

add_executable(protocol_envelopes protocol.envelopes.cpp)
target_include_directories(protocol_envelopes PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(protocol_envelopes PRIVATE ym_test_core)
set_target_properties(
    protocol_envelopes
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
)
add_test(NAME protocol.envelopes COMMAND protocol_envelopes)
ym_build_controls(protocol_envelopes)

add_executable(protocol_response_errors protocol.response_errors.cpp)
target_include_directories(protocol_response_errors PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(protocol_response_errors PRIVATE ym_test_core)
set_target_properties(
    protocol_response_errors
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
)
add_test(NAME protocol.response_errors COMMAND protocol_response_errors)
ym_build_controls(protocol_response_errors)

add_executable(abi_response_decode abi.response_decode.cpp)
target_include_directories(abi_response_decode PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(abi_response_decode PRIVATE ym_test_core)
set_target_properties(
    abi_response_decode
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
)
add_test(NAME abi.response_decode COMMAND abi_response_decode)
ym_build_controls(abi_response_decode)

add_executable(protocol_response_metadata protocol.response_metadata.cpp)
target_include_directories(protocol_response_metadata PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(protocol_response_metadata PRIVATE ym_test_core)
set_target_properties(
    protocol_response_metadata
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
)
add_test(NAME protocol.response_metadata COMMAND protocol_response_metadata)
ym_build_controls(protocol_response_metadata)

add_executable(cpp_response_decode cpp.response_decode.cpp)
target_include_directories(cpp_response_decode PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(cpp_response_decode PRIVATE ym_test_core)
set_target_properties(
    cpp_response_decode
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
)
add_test(NAME cpp.response_decode COMMAND cpp_response_decode)
ym_build_controls(cpp_response_decode)

add_executable(protocol_response_conformance protocol.response_conformance.cpp)
target_include_directories(protocol_response_conformance PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(protocol_response_conformance PRIVATE ym_test_core)
set_target_properties(
    protocol_response_conformance
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
)
add_test(NAME protocol.response_conformance COMMAND protocol_response_conformance)
ym_build_controls(protocol_response_conformance)

add_executable(transport_curl_runtime transport.curl_runtime.cpp)
target_include_directories(transport_curl_runtime PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(transport_curl_runtime PRIVATE ym_test_core)
set_target_properties(
    transport_curl_runtime
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
)
add_test(NAME transport.curl_runtime COMMAND transport_curl_runtime)
ym_build_controls(transport_curl_runtime)

add_executable(transport_body_sink transport.body_sink.cpp)
target_include_directories(transport_body_sink PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(transport_body_sink PRIVATE ym_test_core)
set_target_properties(
    transport_body_sink
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
)
add_test(NAME transport.body_sink COMMAND transport_body_sink)
ym_build_controls(transport_body_sink)

add_executable(transport_header_sink transport.header_sink.cpp)
target_include_directories(transport_header_sink PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(transport_header_sink PRIVATE ym_test_core)
set_target_properties(
    transport_header_sink
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
)
add_test(NAME transport.header_sink COMMAND transport_header_sink)
ym_build_controls(transport_header_sink)

add_executable(transport_url_policy transport.url_policy.cpp)
target_include_directories(transport_url_policy PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(transport_url_policy PRIVATE ym_test_core)
set_target_properties(
    transport_url_policy
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
)
add_test(NAME transport.url_policy COMMAND transport_url_policy)
ym_build_controls(transport_url_policy)

add_executable(transport_request_policy transport.request_policy.cpp)
target_include_directories(transport_request_policy PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(transport_request_policy PRIVATE ym_test_core)
set_target_properties(
    transport_request_policy
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
)
add_test(NAME transport.request_policy COMMAND transport_request_policy)
ym_build_controls(transport_request_policy)

add_executable(transport_multi_owner transport.multi_owner.cpp)
target_include_directories(transport_multi_owner PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(transport_multi_owner PRIVATE ym_test_core)
set_target_properties(
    transport_multi_owner
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
)
add_test(NAME transport.multi_owner COMMAND transport_multi_owner)
ym_build_controls(transport_multi_owner)

add_executable(transport_multi_failure transport.multi_failure.cpp)
target_include_directories(transport_multi_failure PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(transport_multi_failure PRIVATE ym_test_core)
set_target_properties(
    transport_multi_failure
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
)
add_test(NAME transport.multi_failure COMMAND transport_multi_failure)
ym_build_controls(transport_multi_failure)

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
    abi.request_owner
    engine.cancellation
    engine.limits
    engine.callbacks
    engine.callback_lifetimes
    cpp.request_owner
    text.utf8
    protocol.percent
    protocol.fields
    protocol.uri
    protocol.form_body
    protocol.json_body
    protocol.headers
    protocol.json_read
    protocol.envelopes
    protocol.response_errors
    abi.response_decode
    protocol.response_metadata
    cpp.response_decode
    protocol.response_conformance
    transport.curl_runtime
    transport.body_sink
    transport.header_sink
    transport.url_policy
    transport.request_policy
    transport.multi_owner
    transport.multi_failure
    PROPERTIES ENVIRONMENT_MODIFICATION "PATH=path_list_prepend:$<TARGET_FILE_DIR:yandex_music::c>"
)

find_package(Python3 COMPONENTS Interpreter REQUIRED)

add_executable(transport_http transport_http.cpp)
target_link_libraries(transport_http PRIVATE ym_test_core)
ym_build_controls(transport_http)
add_test(
    NAME transport.http
    COMMAND
        "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/http_fixture.py"
        "$<TARGET_FILE:transport_http>"
)

add_executable(api_request api.request.cpp)
target_link_libraries(api_request PRIVATE ym_test_core)
add_test(NAME api.request COMMAND api_request)
ym_build_controls(api_request)

add_executable(api_wait api.wait.cpp)
target_link_libraries(api_wait PRIVATE ym_test_core)
add_test(NAME api.wait COMMAND api_wait)
ym_build_controls(api_wait)

add_executable(api_search api.search.cpp)
target_link_libraries(api_search PRIVATE ym_test_core)
add_test(NAME api.search COMMAND api_search)
ym_build_controls(api_search)

add_executable(api_tracks api.tracks.cpp)
target_link_libraries(api_tracks PRIVATE ym_test_core)
add_test(NAME api.tracks COMMAND api_tracks)
ym_build_controls(api_tracks)

add_executable(api_artists api.artists.cpp)
target_link_libraries(api_artists PRIVATE ym_test_core)
add_test(NAME api.artists COMMAND api_artists)
ym_build_controls(api_artists)

add_executable(api_albums api.albums.cpp)
target_link_libraries(api_albums PRIVATE ym_test_core)
add_test(NAME api.albums COMMAND api_albums)
ym_build_controls(api_albums)

add_executable(api_account api.account.cpp)
target_link_libraries(api_account PRIVATE ym_test_core)
add_test(NAME api.account COMMAND api_account)
ym_build_controls(api_account)

add_executable(api_playlists api.playlists.cpp)
target_link_libraries(api_playlists PRIVATE ym_test_core)
add_test(NAME api.playlists COMMAND api_playlists)
ym_build_controls(api_playlists)

add_executable(api_playlist_editing api.playlist_editing.cpp)
target_link_libraries(api_playlist_editing PRIVATE ym_test_core)
add_test(NAME api.playlist_editing COMMAND api_playlist_editing)
ym_build_controls(api_playlist_editing)

add_executable(api_recommendations api.recommendations.cpp)
target_link_libraries(api_recommendations PRIVATE ym_test_core)
add_test(NAME api.recommendations COMMAND api_recommendations)
ym_build_controls(api_recommendations)
