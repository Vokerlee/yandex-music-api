function(expect_success)
    execute_process(
        COMMAND ${ARGV}
        RESULT_VARIABLE code
        OUTPUT_VARIABLE output
        ERROR_VARIABLE errors
    )

    if(NOT code EQUAL 0)
        message(FATAL_ERROR "Curl dependency contract failed: ${output}${errors}")
    endif()
endfunction()

expect_success("${CMAKE_COMMAND}" -S "${YM_SOURCE}/tests/curl-dependency" -B "${YM_BINARY}/parent" -DYM_SOURCE=${YM_SOURCE} -DYM_PARENT_PROVIDER=ON -DYM_CURL_DEPENDENCY_MODE=SYSTEM -DCMAKE_DISABLE_FIND_PACKAGE_CURL=ON -DFETCHCONTENT_FULLY_DISCONNECTED=ON)

execute_process(
    COMMAND
        "${CMAKE_COMMAND}" -S "${YM_SOURCE}/tests/curl-dependency" -B "${YM_BINARY}/missing"
        -DYM_SOURCE=${YM_SOURCE} -DYM_CURL_DEPENDENCY_MODE=SYSTEM
        -DCMAKE_DISABLE_FIND_PACKAGE_CURL=ON
    RESULT_VARIABLE code
    OUTPUT_VARIABLE output
    ERROR_VARIABLE errors
)

string(REGEX REPLACE "[ \t\r\n]+" " " errors "${errors}")

if(code EQUAL 0 OR NOT errors MATCHES "SYSTEM mode never downloads")
    message(FATAL_ERROR "Missing Curl did not fail without downloading: ${output}${errors}")
endif()

execute_process(
    COMMAND
        "${CMAKE_COMMAND}" -S "${YM_SOURCE}/tests/curl-dependency" -B "${YM_BINARY}/invalid"
        -DYM_SOURCE=${YM_SOURCE} -DYM_PARENT_PROVIDER=ON -DYM_CURL_DEPENDENCY_MODE=invalid
    RESULT_VARIABLE code
    OUTPUT_QUIET
    ERROR_QUIET
)

if(code EQUAL 0)
    message(FATAL_ERROR "Invalid Curl dependency mode was accepted")
endif()
