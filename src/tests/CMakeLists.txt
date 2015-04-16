set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

set(KReportTest_SRCS KReportTest.cpp)

add_executable(KoReportTestNAME KReportTest ${KoReportTest_SRCS})
add_test(KReportTest KReportTest)
ecm_mark_as_test(KReportTest)

target_link_libraries(KReportTest
    KReport
    Qt5::Test
)
