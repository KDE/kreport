ecm_create_qm_loader(koreport_mapsplugin_QM_LOADER kreport_mapsplugin_qt)

#build a shared library
set(kreport_mapsplugin_LIB_SRCS
    ${PROJECT_SOURCE_DIR}/src/common/kreportplugin_debug.cpp
    KoReportDesignerItemMaps.cpp
    KoReportItemMaps.cpp
    KoReportMapsPlugin.cpp
    krscriptmaps.cpp
    MapRenderer.cpp
    ${koreport_mapsplugin_QM_LOADER}
)

add_library(kreport_mapsplugin MODULE ${kreport_mapsplugin_LIB_SRCS})
kcoreaddons_desktop_to_json(kreport_mapsplugin kreport_mapsplugin.desktop)

target_link_libraries(kreport_mapsplugin
    PUBLIC
        KReport
    PRIVATE
        ${MARBLE_LIBRARIES}
)

########### install files ###############

install(TARGETS kreport_mapsplugin DESTINATION ${KREPORT_PLUGIN_INSTALL_DIR})

add_subdirectory(pics)
