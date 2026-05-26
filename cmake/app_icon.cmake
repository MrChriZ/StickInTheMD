# Platform application icons for stickinthemd

set(STICKINTHEMD_ICON_DIR "${CMAKE_SOURCE_DIR}/assets/icon")

if(WIN32)
  set(STICKINTHEMD_WIN_RC "${CMAKE_SOURCE_DIR}/resources/windows/app.rc")
  if(EXISTS "${STICKINTHEMD_ICON_DIR}/stickinthemd.ico" AND EXISTS "${STICKINTHEMD_WIN_RC}")
    target_sources(stickinthemd PRIVATE "${STICKINTHEMD_WIN_RC}")
    set_source_files_properties("${STICKINTHEMD_WIN_RC}" PROPERTIES
      OBJECT_DEPENDS "${STICKINTHEMD_ICON_DIR}/stickinthemd.ico")
  endif()
endif()

if(APPLE)
  set_target_properties(stickinthemd PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_BUNDLE_NAME "StickInTheMD"
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.stickinthemd.app"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}"
    MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
    MACOSX_BUNDLE_ICON_FILE "stickinthemd.icns"
  )
  set(STICKINTHEMD_MAC_ICNS "${STICKINTHEMD_ICON_DIR}/stickinthemd.icns")
  if(EXISTS "${STICKINTHEMD_MAC_ICNS}")
    set_source_files_properties("${STICKINTHEMD_MAC_ICNS}" PROPERTIES
      MACOSX_PACKAGE_LOCATION "Resources")
    target_sources(stickinthemd PRIVATE "${STICKINTHEMD_MAC_ICNS}")
  endif()
endif()

if(UNIX AND NOT APPLE)
  install(FILES "${STICKINTHEMD_ICON_DIR}/stickinthemd-256.png"
    DESTINATION share/icons/hicolor/256x256/apps
    RENAME stickinthemd.png
    OPTIONAL)
  install(FILES "${CMAKE_SOURCE_DIR}/packaging/linux/stickinthemd.desktop"
    DESTINATION share/applications
    OPTIONAL)
endif()
