cmake_minimum_required(VERSION 3.28)

project(curl_contract LANGUAGES C)

if(YM_PARENT_PROVIDER)
    add_library(CURL::libcurl INTERFACE IMPORTED)
endif()

include("${YM_SOURCE}/cmake/curl.cmake")

ym_resolve_curl()

if(NOT TARGET CURL::libcurl)
    message(FATAL_ERROR "Resolver did not supply CURL::libcurl")
endif()
