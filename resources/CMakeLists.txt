file(COPY word-lists DESTINATION ${CMAKE_BINARY_DIR}/resources)

find_package(Qt6 REQUIRED COMPONENTS LinguistTools)
qt_standard_project_setup(I18N_TRANSLATED_LANGUAGES fr)
qt_add_translations(Word-finder)

target_sources(Word-finder PRIVATE icon.rc)
qt_add_resources(Word-finder imageresources
    PREFIX "/images"
    FILES icon.png
)