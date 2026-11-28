ym_test(transport.contract contract.cpp ym_test_core)
ym_test(transport.curl_runtime curl_runtime.cpp ym_test_core)
ym_test(transport.body_sink body_sink.cpp ym_test_core)
ym_test(transport.header_sink header_sink.cpp ym_test_core)
ym_test(transport.url_policy url_policy.cpp ym_test_core)
ym_test(transport.request_policy request_policy.cpp ym_test_core)
ym_test(transport.multi_owner multi_owner.cpp ym_test_core)
ym_test(transport.multi_failure multi_failure.cpp ym_test_core)

ym_test_target(transport_http http.cpp ym_test_core)
add_test(
    NAME transport.http
    COMMAND
        "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/http_fixture.py"
        $<TARGET_FILE:transport_http>
)
