function(run)
    execute_process(COMMAND ${ARGV} RESULT_VARIABLE result COMMAND_ECHO STDOUT)

    if(NOT result EQUAL 0)
        message(FATAL_ERROR "Consumer command failed: ${ARGV}")
    endif()
endfunction()

run("${CMAKE_COMMAND}" -S "${YM_SOURCE}/tests/consumers" -B "${YM_BINARY}" -DYM_SOURCE=${YM_SOURCE} -DYM_BUILD_TESTS=OFF -DBUILD_SHARED_LIBS=${YM_SHARED} -DCMAKE_BUILD_TYPE=Debug -DFETCHCONTENT_SOURCE_DIR_NLOHMANN_JSON=${YM_JSON_SOURCE} "-DCMAKE_PREFIX_PATH=${YM_PREFIX_PATH}" -DYM_DEPENDENCY_MODE=${YM_MODE})

run("${CMAKE_COMMAND}" --build "${YM_BINARY}" --config Debug --parallel 2)

run("${CMAKE_CTEST_COMMAND}" --test-dir "${YM_BINARY}" -C Debug --output-on-failure --no-tests=error)
