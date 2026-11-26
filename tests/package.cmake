function(run)
    execute_process(COMMAND ${ARGV} RESULT_VARIABLE result COMMAND_ECHO STDOUT)

    if(NOT result EQUAL 0)
        message(FATAL_ERROR "Package command failed: ${ARGV}")
    endif()
endfunction()

# This directory is test-owned scratch storage beneath the current binary directory.
file(REMOVE_RECURSE "${YM_BINARY}")

file(MAKE_DIRECTORY "${YM_BINARY}/source")

file(
    COPY
        "${YM_SOURCE}/CMakeLists.txt"
        "${YM_SOURCE}/LICENSE"
        "${YM_SOURCE}/src"
        "${YM_SOURCE}/cmake"
    DESTINATION "${YM_BINARY}/source"
)

run("${CMAKE_COMMAND}" -S "${YM_BINARY}/source" -B "${YM_BINARY}/build" -DYM_BUILD_TESTS=OFF -DYM_INSTALL=ON -DBUILD_SHARED_LIBS=${YM_SHARED} -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=${YM_BINARY}/prefix-a)

run("${CMAKE_COMMAND}" --build "${YM_BINARY}/build" --config Debug --parallel 2)

run("${CMAKE_COMMAND}" --install "${YM_BINARY}/build" --config Debug)

file(COPY "${YM_BINARY}/prefix-a/" DESTINATION "${YM_BINARY}/prefix-b")

run("${CMAKE_COMMAND}" -S "${YM_SOURCE}/tests/consumers" -B "${YM_BINARY}/consumer" -DYM_INSTALLED=ON -DCMAKE_PREFIX_PATH=${YM_BINARY}/prefix-b)

run("${CMAKE_COMMAND}" --build "${YM_BINARY}/consumer" --config Debug --parallel 2)

run("${CMAKE_CTEST_COMMAND}" --test-dir "${YM_BINARY}/consumer" -C Debug --output-on-failure --no-tests=error)
