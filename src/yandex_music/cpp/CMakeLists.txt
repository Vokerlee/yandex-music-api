add_library(yandex_music_cpp INTERFACE)
add_library(yandex_music::cpp ALIAS yandex_music_cpp)
target_compile_features(yandex_music_cpp INTERFACE cxx_std_23)
target_link_libraries(yandex_music_cpp INTERFACE yandex_music_c)
set_target_properties(yandex_music_cpp PROPERTIES EXPORT_NAME cpp)
target_sources(
    yandex_music_cpp
    INTERFACE
        FILE_SET HEADERS
            BASE_DIRS "${PROJECT_SOURCE_DIR}/src"
            FILES
                client.hpp
                download.hpp
                error.hpp
                ids.hpp
                request.hpp
                response.hpp
                result.hpp
                yandex_music.hpp
)

if(TARGET ym_test_core)
    add_subdirectory(tests)
endif()
