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
        KReport
    PRIVATE
        Qt5::Test
)
generate_export_header(KReportTestUtils)

find_package(Qt5Test REQUIRED)

macro(KREPORT_UNIT_TESTS)
  foreach(_testname ${ARGN})
    ecm_add_test(${_testname}.cpp
        TEST_NAME ${_testname}
        LINK_LIBRARIES KReportTestUtils KPropertyWidgets Qt5::Test KF5::WidgetsAddons)
  endforeach()
endmacro(KREPORT_UNIT_TESTS)

kreport_unit_tests(
    ElementsTest
    PluginsTest
)

add_subdirectory(headers)
add_subdirectory(format)
