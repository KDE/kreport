ecm_create_qm_loader(kreport_mapsplugin_QM_LOADER kreport_mapsplugin_qt)

#build a shared library
set(kreport_mapsplugin_LIB_SRCS
    ${PROJECT_SOURCE_DIR}/src/common/kreportplugin_debug.cpp
    KReportDesignerItemMaps.cpp
    KReportItemMaps.cpp
    KReportMapsPlugin.cpp
    KReportMapRenderer.cpp
    ${kreport_mapsplugin_QM_LOADER}
    kreport_mapsplugin.json
)

if(KREPORT_SCRIPTING)
    list(APPEND kreport_mapsplugin_LIB_SRCS
        KReportScriptMaps.cpp
    )
endif()

add_library(org.kde.kreport.maps MODULE ${kreport_mapsplugin_LIB_SRCS})

target_link_libraries(org.kde.kreport.maps
    PUBLIC
        KReport
    PRIVATE
        Marble
)

########### install files ###############

install(TARGETS org.kde.kreport.maps DESTINATION ${KREPORT_PLUGIN_INSTALL_DIR})

add_subdirectory(pics)
