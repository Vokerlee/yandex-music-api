option(YM_ENABLE_ASAN "Instrument SDK and tests with address/undefined sanitizers" OFF)
option(YM_ENABLE_TSAN "Instrument SDK and tests with thread sanitizer" OFF)
if(YM_ENABLE_ASAN AND YM_ENABLE_TSAN)
    message(FATAL_ERROR "ASan and TSan require separate builds")
endif()
if((YM_ENABLE_ASAN OR YM_ENABLE_TSAN) AND NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    message(FATAL_ERROR "The sanitizer options require GCC or Clang")
endif()
function(ym_build_controls target)
    target_compile_options(
        ${target}
        PRIVATE
            $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall;-Wextra;-Wpedantic>
            $<$<CXX_COMPILER_ID:MSVC>:/W4>
    )
    if(YM_ENABLE_ASAN)
        target_compile_options(
            ${target}
            PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer
        )
        target_link_options(${target} PRIVATE -fsanitize=address,undefined)
    elseif(YM_ENABLE_TSAN)
        target_compile_options(${target} PRIVATE -fsanitize=thread -fno-omit-frame-pointer)
        target_link_options(${target} PRIVATE -fsanitize=thread)
    endif()
endfunction()
