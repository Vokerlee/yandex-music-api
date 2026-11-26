include(FetchContent)

set(YM_CURL_DEPENDENCY_MODE AUTO CACHE STRING "Curl resolution: AUTO, SYSTEM or FETCH")
set_property(CACHE YM_CURL_DEPENDENCY_MODE PROPERTY STRINGS AUTO SYSTEM FETCH)

if(NOT YM_CURL_DEPENDENCY_MODE MATCHES "^(AUTO|SYSTEM|FETCH)$")
    message(FATAL_ERROR "YM_CURL_DEPENDENCY_MODE must be AUTO, SYSTEM or FETCH")
endif()

function(ym_resolve_curl)
    if(TARGET CURL::libcurl)
        return()
    endif()

    if(NOT YM_CURL_DEPENDENCY_MODE STREQUAL "FETCH")
        find_package(CURL 8.5 QUIET)
    endif()

    if(TARGET CURL::libcurl)
        return()
    endif()

    if(YM_CURL_DEPENDENCY_MODE STREQUAL "SYSTEM")
        message(
            FATAL_ERROR
            "Install libcurl >= 8.5 with TLS and asynchronous DNS or supply CURL::libcurl; SYSTEM mode never downloads dependencies"
        )
    endif()

    # Function scope preserves the embedding project's options.
    set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
    set(BUILD_SHARED_LIBS OFF)
    set(BUILD_STATIC_LIBS ON)
    set(BUILD_CURL_EXE OFF)
    set(BUILD_TESTING OFF)
    set(BUILD_LIBCURL_DOCS OFF)
    set(BUILD_MISC_DOCS OFF)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
    set(CURL_ENABLE_EXPORT_TARGET ON)

    if(DEFINED YM_INSTALL AND NOT YM_INSTALL)
        set(CURL_DISABLE_INSTALL ON)
    endif()

    set(ENABLE_THREADED_RESOLVER ON)
    set(CURL_ENABLE_SSL ON)

    if(WIN32)
        set(CURL_USE_SCHANNEL ON)
        set(CURL_USE_OPENSSL OFF)
    else()
        set(CURL_USE_OPENSSL ON)
    endif()

    set(CURL_USE_LIBPSL OFF)
    set(CURL_ZLIB OFF)
    set(CURL_BROTLI OFF)
    set(CURL_ZSTD OFF)
    set(CURL_USE_LIBSSH2 OFF)
    set(CURL_USE_LIBIDN2 OFF)
    set(USE_NGHTTP2 OFF)

    foreach(
        protocol
        DICT
        FILE
        FTP
        GOPHER
        IMAP
        LDAP
        LDAPS
        MQTT
        POP3
        RTSP
        SMB
        SMTP
        TELNET
        TFTP
        WEBSOCKETS
    )
        set(CURL_DISABLE_${protocol} ON)
    endforeach()

    FetchContent_Declare(
        ym_curl
        URL https://curl.se/download/curl-8.23.0.tar.xz
        URL_HASH SHA256=8dadd7d2536a314f982ad05a651b713f426bf7d02cc2c76cffb9796f2218e6c5
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )

    FetchContent_MakeAvailable(ym_curl)
endfunction()
