# Options
simple_option(KREPORT_SCRIPTING "Scripting support using JavaScript language" ON)
configure_file(config-kreport.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-kreport.h)

if(KREPORT_SCRIPTING)
    find_package(Qt5Qml REQUIRED)
    set(SCRIPTING_LIBS Qt5::Qml)
endif()

add_subdirectory(plugins)
add_subdirectory(pics)

set(kreport_LIB_SRCS
    common/kreport_debug.cpp
    common/kreportplugin_debug.cpp
    common/KReportPosition.cpp
    common/KReportSize.cpp
    common/KReportItemBase.cpp
    common/KReportAsyncItemBase.cpp
    common/KReportSectionData.cpp
    common/KReportLabelSizeInfo.cpp
    common/KReportPageOptions.cpp
    common/KReportDocument.cpp
    common/KReportDetailSectionData.cpp
    common/KReportPluginInterface.cpp
    common/KReportItemLine.cpp
    common/KReportRenderObjects.cpp
    common/KReportPluginManager.cpp
    common/KReportJsonTrader_p.cpp
    common/KReportPluginMetaData.cpp
    common/KReportData.cpp
    common/KReportUtils.cpp
    common/KReportPageFormat.cpp
    common/KReportUnit.cpp
    common/KReportDesign.cpp
    common/KReportDesign_p.cpp
    common/KReportLineStyle.cpp
    common/KReportElement.cpp
    ${PROJECT_BINARY_DIR}/src/KReportElement_sdc.cpp
    common/KReportSection.cpp

    renderer/KReportPrintRenderer_p.cpp
    renderer/KReportPreRenderer.cpp
    renderer/KReportAsyncItemManager_p.cpp
    renderer/KReportScreenRenderer_p.cpp
    renderer/KReportHTMLTableRenderer_p.cpp
    renderer/KReportHTMLCSSRenderer_p.cpp
    #TODO renderer/KReportKSpreadRenderer.cpp
    #TODO renderer/KReportODTRenderer.cpp
    #TODO renderer/KOdtFrameReportRenderer.cpp
    renderer/KReportRendererBase.cpp
    renderer/KReportPage.cpp
    renderer/KReportView.cpp
    renderer/KReportOneRecordData_p.cpp

    wrtembed/KReportDetailGroupSectionDialog.cpp
    wrtembed/KReportDesignerItemBase.cpp
    wrtembed/KReportDesignerItemRectBase.cpp
    wrtembed/KReportDesignerItemLine.cpp
    wrtembed/KReportDesignerSection.cpp
    wrtembed/KReportDesignerSectionDetailGroup.cpp
    wrtembed/KReportDesignerSectionDetail.cpp
    wrtembed/KReportDesignerSectionScene.cpp
    wrtembed/KReportDesignerSectionView.cpp
    wrtembed/KReportDesigner.cpp
    wrtembed/KReportPropertiesButton.cpp
    wrtembed/KReportSectionEditor.cpp
    wrtembed/KReportDpi.cpp
    wrtembed/KReportRuler.cpp
    wrtembed/KReportZoomHandler.cpp
    wrtembed/KReportZoomMode.cpp

    items/label/KReportItemLabel.cpp
    items/label/KReportDesignerItemLabel.cpp
    items/label/KReportLabelPlugin.cpp
    items/label/KReportBoundedTextItem.cpp
    items/label/KReportLabelElement.cpp

    items/check/KReportItemCheck.cpp
    items/check/KReportDesignerItemCheckBox.cpp
    items/check/KReportCheckBoxPlugin.cpp

    items/field/KReportItemField.cpp
    items/field/KReportDesignerItemField.cpp
    items/field/KReportFieldPlugin.cpp

    items/image/KReportItemImage.cpp
    items/image/KReportDesignerItemImage.cpp
    items/image/KReportImagePlugin.cpp

    items/text/KReportItemText.cpp
    items/text/KReportDesignerItemText.cpp
    items/text/KReportTextPlugin.cpp
)

if(KREPORT_SCRIPTING)
    list(APPEND kreport_LIB_SRCS
        renderer/scripting/KReportScriptHandler.cpp
        renderer/scripting/KReportScriptConstants.cpp
        renderer/scripting/KReportScriptDebug.cpp
        renderer/scripting/KReportScriptDraw.cpp
        renderer/scripting/KReportScriptReport.cpp
        renderer/scripting/KReportScriptSection.cpp
        renderer/scripting/KReportScriptLine.cpp

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

        items/label/KReportScriptLabel.cpp
        items/check/KReportScriptCheck.cpp
        items/field/KReportScriptField.cpp
        items/image/KReportScriptImage.cpp
        items/text/KReportScriptText.cpp
    )
endif()

qt5_wrap_ui(kreport_LIB_SRCS
    wrtembed/KReportDetailGroupSectionDialog.ui
    wrtembed/KReportSectionEditor.ui
)

qt_wrap_cpp(KReport kreport_LIB_SRCS
    renderer/scripting/KReportGroupTracker.h
    )

ecm_create_qm_loader(kreport_LIB_SRCS kreport_qt)

add_library(KReport SHARED ${kreport_LIB_SRCS})

kdb_create_shared_data_classes(
    kreport_GENERATED_SHARED_DATA_CLASS_HEADERS # output variable with list of headers
    NO_PREFIX # subdirectory in which the headers should be generated
    common/KReportLineStyle.shared.h
    common/KReportElement.shared.h
    common/KReportSection.shared.h
    items/label/KReportLabelElement.shared.h
)

kdb_remove_extensions(
    kreport_GENERATED_SHARED_DATA_CLASS_BASENAMES
    ${kreport_GENERATED_SHARED_DATA_CLASS_HEADERS}
)

generate_export_header(KReport)

#qt5_use_modules(KReport Widgets Xml PrintSupport)

set(kreport_INCLUDE_DIRS
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/kreport
    ${CMAKE_CURRENT_BINARY_DIR}/kreport/common
    ${CMAKE_CURRENT_BINARY_DIR}/kreport/wrtembed
    ${CMAKE_CURRENT_BINARY_DIR}/kreport/renderer
    ${CMAKE_CURRENT_BINARY_DIR}/kreport/renderer/scripting
    ${CMAKE_CURRENT_SOURCE_DIR}/common
    ${CMAKE_CURRENT_SOURCE_DIR}/wrtembed
    ${CMAKE_CURRENT_SOURCE_DIR}/renderer
    ${CMAKE_CURRENT_SOURCE_DIR}/renderer/scripting
)

target_include_directories(KReport
    PUBLIC "$<BUILD_INTERFACE:${kreport_INCLUDE_DIRS}>"
    INTERFACE "$<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}/KReport>"
    PRIVATE common renderer renderer/scripting wrtembed
)

target_link_libraries(KReport
    PUBLIC
        Qt5::PrintSupport
        Qt5::Xml
        KF5::CoreAddons
        KProperty
    PRIVATE
        KF5::WidgetsAddons
        KF5::ConfigGui     # KStandardShortcut
        ${SCRIPTING_LIBS}
        #KF5::KIOCore
        #KF5::KIOFileWidgets
        #KF5::KIOWidgets
        #KF5::KIONTLM
)

set_target_properties(KReport
    PROPERTIES VERSION 1.0
               SOVERSION 1.0.0
               EXPORT_NAME KReport
)

ecm_setup_version(${KREPORT_VERSION}
    VARIABLE_PREFIX KREPORT
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/kreport_version.h"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KReportConfigVersion.cmake"
)

install(TARGETS KReport
        EXPORT KReportTargets
        ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

# Create a Config.cmake and a ConfigVersion.cmake file and install them
set(CMAKECONFIG_INSTALL_DIR "${CMAKECONFIG_INSTALL_PREFIX}/KReport")

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

install(EXPORT KReportTargets
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        FILE KReportTargets.cmake)

ecm_generate_pri_file(
    BASE_NAME KReport
    LIB_NAME KReport
    DEPS "widgets"
    FILENAME_VAR PRI_FILENAME
    INCLUDE_INSTALL_DIR ${INCLUDE_INSTALL_DIR}/KReport
)

install(FILES ${PRI_FILENAME}
        DESTINATION ${ECM_MKSPECS_INSTALL_DIR})

install(FILES kreport_elementplugin.desktop  DESTINATION ${SERVICETYPES_INSTALL_DIR})

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/kreport_version.h"
        DESTINATION "${INCLUDE_INSTALL_DIR}"
        COMPONENT Devel)

ecm_generate_headers(kreport_FORWARDING_HEADERS
    REQUIRED_HEADERS kreport_HEADERS
    ORIGINAL CAMELCASE
    RELATIVE common
    HEADER_NAMES
        KReportPageFormat
        KReportData
        KReportItemBase
        KReportItemLine
        KReportPluginMetaData
        KReportPluginManager
        KReportPluginInterface
        KReportUnit
        KReportUtils
        KReportDesign
        KReportSize
        KReportPosition
        KReportDocument
        KReportSectionData
        KReportPageOptions
        KReportRenderObjects
        KReportAsyncItemBase
)

ecm_generate_headers(kreport_FORWARDING_HEADERS
    REQUIRED_HEADERS kreport_HEADERS
    ORIGINAL CAMELCASE
    RELATIVE renderer
    HEADER_NAMES
        KReportPage
        KReportRendererBase
        KReportPreRenderer
        KReportView
)

ecm_generate_headers(kreport_FORWARDING_HEADERS
    REQUIRED_HEADERS kreport_HEADERS
    ORIGINAL CAMELCASE
    RELATIVE renderer/scripting
    HEADER_NAMES
        KReportScriptHandler
        KReportScriptDraw
        KReportScriptConstants
        KReportGroupTracker
)

ecm_generate_headers(kreport_FORWARDING_HEADERS
    REQUIRED_HEADERS kreport_HEADERS
    ORIGINAL CAMELCASE
    RELATIVE wrtembed
    HEADER_NAMES
        KReportZoomMode
        KReportDesignerSectionDetail
        KReportDesignerSection
        KReportDpi
        KReportZoomHandler
        KReportDesignerItemBase
        KReportDesignerSectionDetailGroup
        KReportDesignerItemRectBase
        KReportDesigner
)

ecm_generate_headers(kreport_FORWARDING_HEADERS_FROM_BUILDDIR
    REQUIRED_HEADERS kreport_HEADERS_FROM_BUILDDIR
    ORIGINAL CAMELCASE
    SOURCE_DIR ${PROJECT_BINARY_DIR}/src
    HEADER_NAMES
        ${kreport_GENERATED_SHARED_DATA_CLASS_BASENAMES}
)

install(
    FILES
        ${kreport_HEADERS}
        ${kreport_HEADERS_FROM_BUILDDIR}
    DESTINATION ${INCLUDE_INSTALL_DIR}/KReport
    COMPONENT Devel
)

install(
    FILES
        ${kreport_FORWARDING_HEADERS}
        ${kreport_FORWARDING_HEADERS_FROM_BUILDDIR}
        ${PROJECT_BINARY_DIR}/src/kreport_export.h
        ${PROJECT_BINARY_DIR}/src/config-kreport.h
    DESTINATION ${INCLUDE_INSTALL_DIR}/KReport
    COMPONENT Devel
)

enable_testing()
