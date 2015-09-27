ecm_create_qm_loader(koreport_chartplugin_QM_LOADER kreport_chartplugin_qt)

set(koreport_chartplugin_LIB_SRCS
    ${PROJECT_SOURCE_DIR}/src/common/kreportplugin_debug.cpp
    KoReportItemChart.cpp
    KoReportDesignerItemChart.cpp
    krscriptchart.cpp
    KoReportChartPlugin.cpp
    ${koreport_chartplugin_QM_LOADER}
)

add_library(koreport_chartplugin MODULE ${koreport_chartplugin_LIB_SRCS})

target_link_libraries(koreport_chartplugin
    ${KOREPORT_LIBS}
    KChart
    KProperty
)

install(TARGETS koreport_chartplugin  DESTINATION ${PLUGIN_INSTALL_DIR})

add_subdirectory(pics)
