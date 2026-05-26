cmake_minimum_required(VERSION 3.16)
project(StickInTheMD VERSION 1.0.0 LANGUAGES C CXX)

find_package(Threads REQUIRED)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# One deployable stickinthemd.exe on Windows (no VC++ redistributable DLLs).
if(WIN32 AND MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

option(STICKINTHEMD_BUILD_APP "Build the StickInTheMD desktop application" ON)
option(STICKINTHEMD_BUILD_TESTS "Build unit tests" ON)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/embed_ui.cmake)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/generated)
set(STICKINTHEMD_UI_EMBEDDED_CPP ${CMAKE_BINARY_DIR}/generated/ui_embedded.cpp)
stickinthemd_embed_ui(
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/ui.html
    ${STICKINTHEMD_UI_EMBEDDED_CPP})

# Vendored markdown parser (no install required)
set(BUILD_MD2HTML_EXECUTABLE OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
add_subdirectory(third_party/md4c)

# Vendored webview (uses OS WebKit / WebView2 — no bundled Chromium)
set(WEBVIEW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(WEBVIEW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(WEBVIEW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(WEBVIEW_BUILD_SHARED_LIBRARY OFF CACHE BOOL "" FORCE)
set(WEBVIEW_BUILD_STATIC_LIBRARY ON CACHE BOOL "" FORCE)
set(WEBVIEW_INSTALL_TARGETS OFF CACHE BOOL "" FORCE)
set(WEBVIEW_USE_STATIC_MSVC_RUNTIME ON CACHE BOOL "" FORCE)
add_subdirectory(third_party/webview)

add_library(stickinthemd_core STATIC
    src/core/markdown_renderer.cpp
    src/core/html_builder.cpp
    src/core/document.cpp
    src/core/document_store.cpp
    src/core/autosave_controller.cpp
    src/core/app_controller.cpp
    src/core/view_mode.cpp
    src/core/color_theme.cpp
    src/core/text_normalize.cpp
    src/core/file_dialogs.cpp
    src/core/pdf_exporter.cpp
    ${STICKINTHEMD_UI_EMBEDDED_CPP}
    third_party/tinyfiledialogs/tinyfiledialogs.c
)
target_include_directories(stickinthemd_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/md4c/src
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/tinyfiledialogs
)
target_link_libraries(stickinthemd_core PUBLIC md4c-html)
target_compile_definitions(stickinthemd_core PRIVATE _CRT_SECURE_NO_WARNINGS)

if(WIN32)
    target_link_libraries(stickinthemd_core PRIVATE comdlg32 ole32)
endif()

if(STICKINTHEMD_BUILD_APP)
    add_executable(stickinthemd src/app/stick_app.cpp)
    target_include_directories(stickinthemd PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/webview/core/include
    )
    target_link_libraries(stickinthemd PRIVATE
        stickinthemd_core
        webview::core
        Threads::Threads
    )
    target_compile_definitions(stickinthemd PRIVATE
        _CRT_SECURE_NO_WARNINGS
        STICKINTHEMD_VERSION="${PROJECT_VERSION}")

    if(APPLE)
        target_link_libraries(stickinthemd PRIVATE "-framework WebKit")
    endif()

    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/app_icon.cmake)

    if(WIN32)
        # GUI subsystem: no console window on launch (keep int main() entry).
        if(MSVC)
            target_link_options(stickinthemd PRIVATE
                "/SUBSYSTEM:WINDOWS"
                "/ENTRY:mainCRTStartup")
        else()
            target_link_options(stickinthemd PRIVATE -mwindows)
        endif()
    endif()

    if(APPLE)
        install(TARGETS stickinthemd BUNDLE DESTINATION .)
    else()
        install(TARGETS stickinthemd RUNTIME DESTINATION .)
    endif()

    if(WIN32)
        include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/windows_installer.cmake)
    endif()
endif()

if(STICKINTHEMD_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
