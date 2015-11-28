ecm_create_qm_loader(kreport_mapsplugin_QM_LOADER kreport_mapsplugin_qt)

#build a shared library
set(kreport_mapsplugin_LIB_SRCS
    ${PROJECT_SOURCE_DIR}/src/common/kreportplugin_debug.cpp
    KReportDesignerItemMaps.cpp
    KReportItemMaps.cpp
    KReportMapsPlugin.cpp
    KReportScriptMaps.cpp
    MapRenderer.cpp
    ${kreport_mapsplugin_QM_LOADER}
)

add_library(kreport_mapsplugin MODULE ${kreport_mapsplugin_LIB_SRCS})

target_link_libraries(kreport_mapsplugin
    PUBLIC
        KReport
    PRIVATE
        ${MARBLE_LIBRARIES}
)

########### install files ###############

install(TARGETS kreport_mapsplugin DESTINATION ${KREPORT_PLUGIN_INSTALL_DIR})

add_subdirectory(pics)
