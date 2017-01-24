add_definitions(-DFILES_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data")

ecm_add_tests(
    FormatTest.cpp
    LINK_LIBRARIES
        KReportTestUtils
)
