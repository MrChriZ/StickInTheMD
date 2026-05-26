# Optional: build the Inno Setup installer after stuckinthemd (requires ISCC on PATH).
option(STUCKINTHEMD_BUILD_WINDOWS_INSTALLER
    "Build StuckInTheMD-*-setup.exe with Inno Setup after stuckinthemd" OFF)

if(NOT STUCKINTHEMD_BUILD_WINDOWS_INSTALLER)
    return()
endif()

find_program(ISCC_EXECUTABLE ISCC
    HINTS
        "$ENV{ProgramFiles(x86)}/Inno Setup 6"
        "$ENV{ProgramFiles}/Inno Setup 6"
    PATH_SUFFIXES ""
    NAMES ISCC ISCC.exe)

if(NOT ISCC_EXECUTABLE)
    message(WARNING "Inno Setup (ISCC) not found; STUCKINTHEMD_BUILD_WINDOWS_INSTALLER ignored")
    return()
endif()

set(STUCKINTHEMD_ISS "${CMAKE_SOURCE_DIR}/packaging/windows/stuckinthemd.iss")
set(STUCKINTHEMD_DIST "${CMAKE_SOURCE_DIR}/dist")

add_custom_command(
    OUTPUT "${STUCKINTHEMD_DIST}/StuckInTheMD-${PROJECT_VERSION}-setup.exe"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${STUCKINTHEMD_DIST}"
    COMMAND "${ISCC_EXECUTABLE}" "${STUCKINTHEMD_ISS}"
        "/DMyAppVersion=${PROJECT_VERSION}"
        "/DMyAppSourceDir=$<TARGET_FILE_DIR:stuckinthemd>"
    DEPENDS stuckinthemd "${STUCKINTHEMD_ISS}"
        "${CMAKE_SOURCE_DIR}/packaging/windows/LICENSE.txt"
        "${CMAKE_SOURCE_DIR}/assets/icon/stuckinthemd.ico"
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/packaging/windows"
    COMMENT "Building Windows installer StuckInTheMD-${PROJECT_VERSION}-setup.exe"
    VERBATIM)

add_custom_target(stuckinthemd_installer
    DEPENDS "${STUCKINTHEMD_DIST}/StuckInTheMD-${PROJECT_VERSION}-setup.exe")
