include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

install(TARGETS yandex_music_c yandex_music_cpp EXPORT yandex_musicTargets
    FILE_SET HEADERS DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
set(config_dir "${CMAKE_INSTALL_LIBDIR}/cmake/yandex_music")
install(EXPORT yandex_musicTargets NAMESPACE yandex_music:: DESTINATION ${config_dir})
configure_package_config_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/yandex_musicConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/yandex_musicConfig.cmake" INSTALL_DESTINATION ${config_dir})
write_basic_package_version_file("${CMAKE_CURRENT_BINARY_DIR}/yandex_musicConfigVersion.cmake"
    VERSION ${PROJECT_VERSION} COMPATIBILITY SameMinorVersion)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/yandex_musicConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/yandex_musicConfigVersion.cmake" DESTINATION ${config_dir})
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE" DESTINATION ${CMAKE_INSTALL_DATADIR}/licenses/yandex_music)
