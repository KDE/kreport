include_directories(
    ${MARBLE_INCLUDE_DIR}
    ${KOREPORT_INCLUDES}
)

#build a shared library
set(koreport_mapsplugin_LIB_SRCS
    KoReportDesignerItemMaps.cpp
    KoReportItemMaps.cpp
    KoReportMapsPlugin.cpp
    krscriptmaps.cpp
    MapRenderer.cpp
)

kde4_add_plugin(koreport_mapsplugin ${koreport_mapsplugin_LIB_SRCS})

target_link_libraries(koreport_mapsplugin
    ${KOREPORT_LIBS}
    ${MARBLE_LIBRARIES}
    KProperty
)

########### install files ###############

install(TARGETS koreport_mapsplugin DESTINATION ${PLUGIN_INSTALL_DIR})
install( FILES  koreport_mapsplugin.desktop  DESTINATION ${SERVICES_INSTALL_DIR}/calligra)
ecm_install_icons(${DATA_INSTALL_DIR}/calligra/icons)
