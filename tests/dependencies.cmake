function(run)
    cmake_parse_arguments(PARSE_ARGV 0 command "" "" "")

    execute_process(
        COMMAND ${command_UNPARSED_ARGUMENTS}
        RESULT_VARIABLE result
        COMMAND_ECHO STDOUT
    )

    if(NOT result EQUAL 0)
        message(FATAL_ERROR "Dependency contract failed: ${ARGV}")
    endif()
endfunction()

run("${CMAKE_COMMAND}" -S "${YM_JSON_SOURCE}" -B "${YM_BINARY}/json" -DJSON_BuildTests=OFF -DJSON_Install=ON -DJSON_MultipleHeaders=ON -DCMAKE_INSTALL_PREFIX=${YM_BINARY}/prefix)
run("${CMAKE_COMMAND}" --install "${YM_BINARY}/json")

foreach(mode SYSTEM AUTO)
    run("${CMAKE_COMMAND}" -S "${YM_SOURCE}" -B "${YM_BINARY}/${mode}" -DYM_DEPENDENCY_MODE=${mode} "-DCMAKE_PREFIX_PATH=${YM_BINARY}/prefix;${YM_PREFIX_PATH}" -DYM_CURL_DEPENDENCY_MODE=${YM_CURL_MODE} -DFETCHCONTENT_SOURCE_DIR_YM_CURL=${YM_CURL_SOURCE} -DYM_BUILD_TESTS=OFF -DFETCHCONTENT_FULLY_DISCONNECTED=ON)
    run("${CMAKE_COMMAND}" --build "${YM_BINARY}/${mode}" --config Debug --parallel 2)
endforeach()

execute_process(
    COMMAND
        "${CMAKE_COMMAND}" -S "${YM_SOURCE}" -B "${YM_BINARY}/invalid" -DYM_DEPENDENCY_MODE=invalid
    RESULT_VARIABLE invalid_result
    OUTPUT_QUIET
    ERROR_QUIET
)

if(invalid_result EQUAL 0)
    message(FATAL_ERROR "Invalid dependency mode was accepted")
endif()

execute_process(
    COMMAND
        "${CMAKE_COMMAND}" -S "${YM_SOURCE}" -B "${YM_BINARY}/missing" -DYM_DEPENDENCY_MODE=SYSTEM
        -DCMAKE_DISABLE_FIND_PACKAGE_nlohmann_json=ON
    RESULT_VARIABLE missing_result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE errors
)

string(REGEX REPLACE "[ \t\r\n]+" " " errors "${errors}")

if(missing_result EQUAL 0 OR NOT errors MATCHES "SYSTEM mode never downloads")
    message(
        FATAL_ERROR
        "Missing system package did not produce the expected diagnostic: ${output}${errors}"
    )
endif()
