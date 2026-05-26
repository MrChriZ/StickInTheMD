# Optional: build the Inno Setup installer after stickinthemd (requires ISCC on PATH).
option(STICKINTHEMD_BUILD_WINDOWS_INSTALLER
    "Build StickInTheMD-*-setup.exe with Inno Setup after stickinthemd" OFF)

if(NOT STICKINTHEMD_BUILD_WINDOWS_INSTALLER)
    return()
endif()

find_program(ISCC_EXECUTABLE ISCC
    HINTS
        "$ENV{ProgramFiles(x86)}/Inno Setup 6"
        "$ENV{ProgramFiles}/Inno Setup 6"
    PATH_SUFFIXES ""
    NAMES ISCC ISCC.exe)

if(NOT ISCC_EXECUTABLE)
    message(WARNING "Inno Setup (ISCC) not found; STICKINTHEMD_BUILD_WINDOWS_INSTALLER ignored")
    return()
endif()

set(STICKINTHEMD_ISS "${CMAKE_SOURCE_DIR}/packaging/windows/stickinthemd.iss")
set(STICKINTHEMD_DIST "${CMAKE_SOURCE_DIR}/dist")

add_custom_command(
    OUTPUT "${STICKINTHEMD_DIST}/StickInTheMD-${PROJECT_VERSION}-setup.exe"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${STICKINTHEMD_DIST}"
    COMMAND "${ISCC_EXECUTABLE}" "${STICKINTHEMD_ISS}"
        "/DMyAppVersion=${PROJECT_VERSION}"
        "/DMyAppSourceDir=$<TARGET_FILE_DIR:stickinthemd>"
    DEPENDS stickinthemd "${STICKINTHEMD_ISS}"
        "${CMAKE_SOURCE_DIR}/packaging/windows/LICENSE.txt"
        "${CMAKE_SOURCE_DIR}/assets/icon/stickinthemd.ico"
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/packaging/windows"
    COMMENT "Building Windows installer StickInTheMD-${PROJECT_VERSION}-setup.exe"
    VERBATIM)

add_custom_target(stickinthemd_installer
    DEPENDS "${STICKINTHEMD_DIST}/StickInTheMD-${PROJECT_VERSION}-setup.exe")
