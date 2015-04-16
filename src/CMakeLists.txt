find_package(Qt5Widgets REQUIRED)
find_package(Qt5Xml REQUIRED)
find_package(Qt5PrintSupport REQUIRED)
find_package(KF5WidgetsAddons REQUIRED)
find_package(KF5Kross REQUIRED)
find_package(KProperty REQUIRED)

# Options
option(KREPORT_SCRIPTING "Enable scripting" OFF)
configure_file(config-kreport.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-kreport.h)

#add_subdirectory(pics)

set(kreport_LIB_SRCS
    common/krutils.cpp
    common/krpos.cpp
    common/krsize.cpp
    common/KoReportItemBase.cpp
    common/KoReportASyncItemBase.cpp
    common/krsectiondata.cpp
    common/labelsizeinfo.cpp
    common/reportpageoptions.cpp
    common/krreportdata.cpp
    common/krdetailsectiondata.cpp
    common/KoReportPluginInterface.cpp
    common/KoReportItemLine.cpp
    common/renderobjects.cpp
    #common/KoReportPluginManager.cpp
    #common/KoReportPluginInfo.cpp
    common/KoReportData.cpp
    common/KoPageFormat.cpp
    common/KoUnit.cpp

    renderer/KoReportPrintRenderer.cpp
    renderer/KoReportPreRenderer.cpp
    renderer/KoReportASyncItemManager.cpp
    renderer/KoReportScreenRenderer.cpp
    renderer/KoReportHTMLTableRenderer.cpp
    renderer/KoReportHTMLCSSRenderer.cpp
    renderer/KoReportKSpreadRenderer.cpp
    renderer/KoReportODTRenderer.cpp
    renderer/KoOdtFrameReportRenderer.cpp
    renderer/KoReportRendererBase.cpp
    renderer/KoReportPage.cpp

    #renderer/scripting/krscripthandler.cpp
    #renderer/scripting/krscriptconstants.cpp
    #renderer/scripting/krscriptdebug.cpp
    #renderer/scripting/krscriptdraw.cpp
    #renderer/scripting/krscriptreport.cpp
    #renderer/scripting/krscriptsection.cpp
    #renderer/scripting/krscriptline.cpp

    #renderer/odtframe/KoOdtFrameReportDocument.cpp
    #renderer/odtframe/KoOdtFrameReportCheckBox.cpp
    #renderer/odtframe/KoOdtFrameReportImage.cpp
    #renderer/odtframe/KoOdtFrameReportTextBox.cpp
    #renderer/odtframe/KoOdtFrameReportLine.cpp
    #renderer/odtframe/KoOdtFrameReportPicture.cpp
    #renderer/odtframe/KoOdtFrameReportPrimitive.cpp

    #renderer/ods/KoSimpleOdsDocument.cpp
    #renderer/ods/KoSimpleOdsSheet.cpp
    #renderer/ods/KoSimpleOdsCell.cpp

    wrtembed/detailgroupsectiondialog.cpp
    wrtembed/KoReportDesignerItemBase.cpp
    wrtembed/KoReportDesignerItemRectBase.cpp
    wrtembed/KoReportDesignerItemLine.cpp
    wrtembed/reportsection.cpp
    wrtembed/reportsectiondetailgroup.cpp
    wrtembed/reportsectiondetail.cpp
    wrtembed/reportscene.cpp
    wrtembed/reportsceneview.cpp
    wrtembed/KoReportDesigner.cpp
    wrtembed/reportpropertiesbutton.cpp
    wrtembed/sectioneditor.cpp
    wrtembed/KoDpi.cpp
    wrtembed/KoRuler.cpp
    wrtembed/KoZoomHandler.cpp
    wrtembed/KoViewConverter.cpp
    wrtembed/KoZoomMode.cpp

    items/label/KoReportItemLabel.cpp
    items/label/KoReportDesignerItemLabel.cpp
    items/label/krscriptlabel.cpp
    items/label/KoReportLabelPlugin.cpp
    items/label/BoundedTextItem.cpp

    items/check/KoReportItemCheck.cpp
    items/check/KoReportDesignerItemCheck.cpp
    items/check/KoReportScriptCheck.cpp
    items/check/KoReportCheckPlugin.cpp

    items/field/KoReportItemField.cpp
    items/field/KoReportDesignerItemField.cpp
    items/field/krscriptfield.cpp
    items/field/KoReportFieldPlugin.cpp

    items/image/KoReportItemImage.cpp
    items/image/KoReportDesignerItemImage.cpp
    items/image/krscriptimage.cpp
    items/image/KoReportImagePlugin.cpp

    items/text/KoReportItemText.cpp
    items/text/KoReportDesignerItemText.cpp
    items/text/krscripttext.cpp
    items/text/KoReportTextPlugin.cpp
)

qt5_wrap_ui(kreport_LIB_SRCS
    wrtembed/detailgroupsectiondialog.ui
    wrtembed/sectioneditor.ui
)

add_library(KReport SHARED ${kreport_LIB_SRCS})

generate_export_header(KReport)

#qt5_use_modules(KReport Widgets Xml PrintSupport)

target_include_directories(KReport
    INTERFACE "$<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}/KReport>"
    PRIVATE common renderer renderer/scripting wrtembed
)

target_link_libraries(KReport
    PUBLIC
        Qt5::PrintSupport
        KF5::WidgetsAddons
        KF5::KrossCore
        KF5::KrossUi
        #KF5::KIOCore
        #KF5::KIOFileWidgets
        #KF5::KIOWidgets
        #KF5::KIONTLM
        KProperty
    PRIVATE
)

set_target_properties(KReport
    PROPERTIES VERSION 1.0
               SOVERSION 1.0.0
               EXPORT_NAME KReport
)

# Create a Config.cmake and a ConfigVersion.cmake file and install them
set(CMAKECONFIG_INSTALL_DIR "${CMAKECONFIG_INSTALL_PREFIX}/KReport")

ecm_setup_version(${KREPORT_VERSION}
    VARIABLE_PREFIX KREPORT
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/kreport_version.h"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KReportConfigVersion.cmake"
)

ecm_configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/KReportConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/KReportConfig.cmake"
    INSTALL_DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
)

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/KReportConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/KReportConfigVersion.cmake"
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel)

#install(TARGETS KReport
#        EXPORT KReportTargets
#        "${INSTALL_TARGETS_DEFAULT_ARGS}")

#install(EXPORT KReportTargets
#        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
#        FILE KReportTargets.cmake)

ecm_generate_pri_file(
    BASE_NAME KReport
    LIB_NAME KReport
    DEPS "widgets"
    FILENAME_VAR PRI_FILENAME
    INCLUDE_INSTALL_DIR ${INCLUDE_INSTALL_DIR}/KReport
)

install(FILES ${PRI_FILENAME}
        DESTINATION ${ECM_MKSPECS_INSTALL_DIR})

#TODO
#install(FILES  koreport_itemplugin.desktop  DESTINATION ${SERVICETYPES_INSTALL_DIR})

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/kreport_version.h"
        DESTINATION "${INCLUDE_INSTALL_DIR}"
        COMPONENT Devel)

ecm_generate_headers(kreport_FORWARDING_HEADERS
    ORIGINAL CAMELCASE
    PREFIX kreport
    HEADER_NAMES
        common/KoPageFormat
        common/KoReportData
        common/KoReportItemBase
        common/KoReportItemLine
        common/KoReportPluginInfo
        common/KoUnit
    REQUIRED_HEADERS kreport_HEADERS
)

install(
    FILES
        ${kreport_HEADERS}
        ${PROJECT_BINARY_DIR}/src/kreport_export.h
        ${PROJECT_BINARY_DIR}/src/config-kreport.h
    DESTINATION ${INCLUDE_INSTALL_DIR}/KReport/kreport
    COMPONENT Devel
)

install(
    FILES
        ${kreport_FORWARDING_HEADERS}
    DESTINATION ${INCLUDE_INSTALL_DIR}/KReport/KReport
    COMPONENT Devel
)

