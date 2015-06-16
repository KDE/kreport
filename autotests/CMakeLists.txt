remove_definitions(
    -DQT_NO_KEYWORDS
    -DQT_NO_SIGNALS_SLOTS_KEYWORDS
    -DQT_NO_CAST_FROM_ASCII
)

include(ECMAddTests)

find_package(Qt5Test REQUIRED)

macro(KREPORT_UNIT_TESTS)
  foreach(_testname ${ARGN})
    ecm_add_test(${_testname}.cpp
        TEST_NAME ${_testname}
        LINK_LIBRARIES KReport KProperty Qt5::Test KF5::WidgetsAddons)
  endforeach()
endmacro(KREPORT_UNIT_TESTS)

kreport_unit_tests(
    PluginsTest
)

add_subdirectory(headers)
