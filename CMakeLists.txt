cmake_minimum_required(VERSION 3.16)
project(StuckInTheMD VERSION 1.0.4 LANGUAGES C CXX)

find_package(Threads REQUIRED)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# One deployable stuckinthemd.exe on Windows (no VC++ redistributable DLLs).
if(WIN32 AND MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

option(STUCKINTHEMD_BUILD_APP "Build the StuckInTheMD desktop application" ON)
option(STUCKINTHEMD_BUILD_TESTS "Build unit tests" ON)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/embed_ui.cmake)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/generated)
set(STUCKINTHEMD_UI_EMBEDDED_CPP ${CMAKE_BINARY_DIR}/generated/ui_embedded.cpp)
stuckinthemd_embed_ui(
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/ui.html
    ${STUCKINTHEMD_UI_EMBEDDED_CPP})

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

add_library(stuckinthemd_core STATIC
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
    ${STUCKINTHEMD_UI_EMBEDDED_CPP}
    third_party/tinyfiledialogs/tinyfiledialogs.c
)
target_include_directories(stuckinthemd_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/md4c/src
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/tinyfiledialogs
)
target_link_libraries(stuckinthemd_core PUBLIC md4c-html)
target_compile_definitions(stuckinthemd_core PRIVATE _CRT_SECURE_NO_WARNINGS)

if(WIN32)
    target_link_libraries(stuckinthemd_core PRIVATE comdlg32 ole32)
endif()

if(STUCKINTHEMD_BUILD_APP)
    add_executable(stuckinthemd src/app/stuck_app.cpp)
    target_include_directories(stuckinthemd PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/webview/core/include
    )
    target_link_libraries(stuckinthemd PRIVATE
        stuckinthemd_core
        webview::core
        Threads::Threads
    )
    target_compile_definitions(stuckinthemd PRIVATE
        _CRT_SECURE_NO_WARNINGS
        STUCKINTHEMD_VERSION="${PROJECT_VERSION}")

    if(APPLE)
        target_link_libraries(stuckinthemd PRIVATE "-framework WebKit")
    endif()

    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/app_icon.cmake)

    if(WIN32)
        # GUI subsystem: no console window on launch (keep int main() entry).
        if(MSVC)
            target_link_options(stuckinthemd PRIVATE
                "/SUBSYSTEM:WINDOWS"
                "/ENTRY:mainCRTStartup")
        else()
            target_link_options(stuckinthemd PRIVATE -mwindows)
        endif()
    endif()

    if(APPLE)
        install(TARGETS stuckinthemd BUNDLE DESTINATION .)
    else()
        install(TARGETS stuckinthemd RUNTIME DESTINATION .)
    endif()

    if(WIN32)
        include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/windows_installer.cmake)
    endif()
endif()

if(STUCKINTHEMD_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
