ecm_create_qm_loader(kreport_chartplugin_QM_LOADER kreport_chartplugin_qt)

set(kreport_chartplugin_LIB_SRCS
    ${PROJECT_SOURCE_DIR}/src/common/kreportplugin_debug.cpp
    KReportItemChart.cpp
    KReportDesignerItemChart.cpp
    KReportChartPlugin.cpp
    ${kreport_chartplugin_QM_LOADER}
)

if(KREPORT_SCRIPTING)
    list(APPEND kreport_chartplugin_LIB_SRCS
        KReportScriptChart.cpp
    )
endif()

add_library(org.kde.kreport.chart MODULE ${kreport_chartplugin_LIB_SRCS})

target_link_libraries(org.kde.kreport.chart
    PUBLIC
        KReport
    PRIVATE
        KChart
)

install(TARGETS org.kde.kreport.chart DESTINATION ${KREPORT_PLUGIN_INSTALL_DIR})

add_subdirectory(pics)
