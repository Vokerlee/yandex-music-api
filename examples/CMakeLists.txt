function(ym_add_example path)
    string(REPLACE "/" "_" name "${path}")

    foreach(language IN ITEMS c cpp)
        set(target "ym_example_${language}_${name}")
        add_executable(${target} "${PROJECT_SOURCE_DIR}/examples/${path}/main.${language}")
        target_link_libraries(${target} PRIVATE "yandex_music::${language}")
        set_target_properties(
            ${target}
            PROPERTIES
                C_EXTENSIONS OFF
                CXX_EXTENSIONS OFF
                RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
        )
        if(language STREQUAL "c")
            target_compile_features(${target} PRIVATE c_std_11)
        endif()
        if(MSVC)
            target_compile_definitions(${target} PRIVATE _CRT_SECURE_NO_WARNINGS)
        endif()
        ym_build_controls(${target})
    endforeach()
endfunction()

add_subdirectory(basics)
add_subdirectory(search)
add_subdirectory(tracks)
add_subdirectory(artists)
add_subdirectory(albums)
add_subdirectory(account)
add_subdirectory(playlists)
