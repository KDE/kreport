remove_definitions(
    -DQT_NO_KEYWORDS
    -DQT_NO_SIGNALS_SLOTS_KEYWORDS
    -DQT_NO_CAST_FROM_ASCII
    -DQT_USE_QSTRINGBUILDER
)

include(ECMAddTests)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# A helper library for report-related tests
add_library(KReportTestUtils SHARED
    KReportTestUtils.cpp
)
target_link_libraries(KReportTestUtils
    PUBLIC
        Qt5::Test
        KReport
)
generate_export_header(KReportTestUtils)

find_package(Qt5Test REQUIRED)

ecm_add_tests(
    ElementsTest.cpp
    PluginsTest.cpp
    LINK_LIBRARIES
        KReportTestUtils
)

if(NOT WIN32) #TODO enable for Windows when headers_test.sh is ported e.g. to python
    add_subdirectory(headers)
endif()
add_subdirectory(format)
