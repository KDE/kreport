include_directories(
    ${PROJECT_BINARY_DIR}/src/kreport/common
    ${PROJECT_BINARY_DIR}/src/kreport/wrtembed
    ${PROJECT_BINARY_DIR}/src/kreport/renderer
    ${PROJECT_BINARY_DIR}/src/kreport/renderer/scripting
    ${PROJECT_SOURCE_DIR}/src/common
    ${PROJECT_SOURCE_DIR}/src/wrtembed
    ${PROJECT_SOURCE_DIR}/src/renderer
    ${PROJECT_SOURCE_DIR}/src/renderer/scripting
)

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
        LINK_LIBRARIES KReport KPropertySet Qt5::Test KF5::WidgetsAddons)
  endforeach()
endmacro(KSERVICE_UNIT_TESTS)

kreport_unit_tests(
    PluginsTest
)
