function(ym_cmake_test name script)
    add_test(
        NAME ${name}
        COMMAND
            "${CMAKE_COMMAND}" -DYM_CURL_SOURCE=${YM_CURL_SOURCE_DIR}
            -DYM_CURL_MODE=${YM_CURL_DEPENDENCY_MODE} -DYM_SOURCE=${PROJECT_SOURCE_DIR}
            -DYM_BINARY=${PROJECT_BINARY_DIR}/${name} -DYM_SHARED=${BUILD_SHARED_LIBS}
            -DYM_JSON_SOURCE=${YM_JSON_SOURCE_DIR} "-DYM_PREFIX_PATH=${CMAKE_PREFIX_PATH}"
            -DYM_MODE=${YM_DEPENDENCY_MODE} -P "${CMAKE_CURRENT_SOURCE_DIR}/${script}"
    )
endfunction()

ym_cmake_test(consumers.source consumer.cmake)
ym_cmake_test(consumers.installed package.cmake)
ym_cmake_test(dependencies.modes dependencies.cmake)
ym_cmake_test(dependencies.curl curl-dependency.cmake)
