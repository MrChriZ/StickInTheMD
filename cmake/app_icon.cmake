# Platform application icons for stuckinthemd

set(STUCKINTHEMD_ICON_DIR "${CMAKE_SOURCE_DIR}/assets/icon")

if(WIN32)
  set(STUCKINTHEMD_WIN_RC "${CMAKE_SOURCE_DIR}/resources/windows/app.rc")
  if(EXISTS "${STUCKINTHEMD_ICON_DIR}/stuckinthemd.ico" AND EXISTS "${STUCKINTHEMD_WIN_RC}")
    target_sources(stuckinthemd PRIVATE "${STUCKINTHEMD_WIN_RC}")
    set_source_files_properties("${STUCKINTHEMD_WIN_RC}" PROPERTIES
      OBJECT_DEPENDS "${STUCKINTHEMD_ICON_DIR}/stuckinthemd.ico")
  endif()
endif()

if(APPLE)
  set_target_properties(stuckinthemd PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_BUNDLE_NAME "StuckInTheMD"
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.stuckinthemd.app"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}"
    MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
    MACOSX_BUNDLE_ICON_FILE "stuckinthemd.icns"
  )
  set(STUCKINTHEMD_MAC_ICNS "${STUCKINTHEMD_ICON_DIR}/stuckinthemd.icns")
  if(EXISTS "${STUCKINTHEMD_MAC_ICNS}")
    set_source_files_properties("${STUCKINTHEMD_MAC_ICNS}" PROPERTIES
      MACOSX_PACKAGE_LOCATION "Resources")
    target_sources(stuckinthemd PRIVATE "${STUCKINTHEMD_MAC_ICNS}")
  endif()
endif()

if(UNIX AND NOT APPLE)
  install(FILES "${STUCKINTHEMD_ICON_DIR}/stuckinthemd-256.png"
    DESTINATION share/icons/hicolor/256x256/apps
    RENAME stuckinthemd.png
    OPTIONAL)
  install(FILES "${CMAKE_SOURCE_DIR}/packaging/linux/stuckinthemd.desktop"
    DESTINATION share/applications
    OPTIONAL)
endif()
