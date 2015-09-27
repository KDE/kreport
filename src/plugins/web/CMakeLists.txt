include_directories(
    ${KOREPORT_INCLUDES}
)

ecm_create_qm_loader(koreport_webplugin_QM_LOADER kreport_webplugin_qt)

#build a shared library
set(koreport_webplugin_LIB_SRCS
    ${PROJECT_SOURCE_DIR}/src/common/kreportplugin_debug.cpp
    KoReportDesignerItemWeb.cpp
    KoReportItemWeb.cpp
    KoReportWebPlugin.cpp
    ${koreport_webplugin_QM_LOADER}
)

add_library(koreport_webplugin MODULE ${koreport_webplugin_LIB_SRCS})

set (QT_USE_QTWEBKIT TRUE)

target_link_libraries(koreport_webplugin
    Qt5::Core
    Qt5::Gui
    Qt5::WebKitWidgets

    KProperty
    ${KOREPORT_LIBS}
)

########### install files ###############

install(TARGETS koreport_webplugin  DESTINATION ${PLUGIN_INSTALL_DIR})

add_subdirectory(pics)
