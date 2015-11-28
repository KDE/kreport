ecm_create_qm_loader(kreport_webplugin_QM_LOADER kreport_webplugin_qt)

#build a shared library
set(kreport_webplugin_LIB_SRCS
    ${PROJECT_SOURCE_DIR}/src/common/kreportplugin_debug.cpp
    KReportDesignerItemWeb.cpp
    KReportItemWeb.cpp
    KReportWebPlugin.cpp
    ${kreport_webplugin_QM_LOADER}
)

add_library(kreport_webplugin MODULE ${kreport_webplugin_LIB_SRCS})

set (QT_USE_QTWEBKIT TRUE)

target_link_libraries(kreport_webplugin
    PUBLIC
        KReport
    PRIVATE
        Qt5::WebKitWidgets # TODO WebEngineWidgets?
)

########### install files ###############

install(TARGETS kreport_webplugin DESTINATION ${KREPORT_PLUGIN_INSTALL_DIR})

add_subdirectory(pics)
