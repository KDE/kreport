ecm_create_qm_loader(kreport_webplugin_QM_LOADER kreport_webplugin_qt)

#build a shared library
set(kreport_webplugin_LIB_SRCS
    ${PROJECT_SOURCE_DIR}/src/common/kreportplugin_debug.cpp
    KReportDesignerItemWeb.cpp
    KReportItemWeb.cpp
    KReportWebPlugin.cpp
    ${kreport_webplugin_QM_LOADER}
)

add_library(org.kde.kreport.web MODULE ${kreport_webplugin_LIB_SRCS})

set (QT_USE_QTWEBKIT TRUE)

target_link_libraries(org.kde.kreport.web
    PUBLIC
        KReport
    PRIVATE
        Qt5::WebKitWidgets # TODO WebEngineWidgets?
)

########### install files ###############

install(TARGETS org.kde.kreport.web DESTINATION ${KREPORT_PLUGIN_INSTALL_DIR})

add_subdirectory(pics)
