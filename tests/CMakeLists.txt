add_executable(stuckinthemd_tests
    test_markdown_renderer.cpp
    test_html_builder.cpp
    test_document.cpp
    test_document_store.cpp
    test_autosave_controller.cpp
    test_app_controller.cpp
    test_pdf_exporter.cpp
    test_json_util.cpp
    test_view_mode.cpp
    test_color_theme.cpp
    test_text_normalize.cpp
    test_link_opener.cpp
)

target_link_libraries(stuckinthemd_tests PRIVATE stuckinthemd_core)
target_include_directories(stuckinthemd_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/third_party/doctest
)

add_test(NAME stuckinthemd_tests COMMAND stuckinthemd_tests)
