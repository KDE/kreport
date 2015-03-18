cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)
project(KReport)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
list(APPEND CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Xml REQUIRED)
find_package(Qt5PrintSupport REQUIRED)
find_package(KF5WidgetsAddons REQUIRED)
find_package(KF5Kross REQUIRED)

#add_subdirectory(pics)
#add_subdirectory(tests)

#build a shared library
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

qt5_wrap_ui(kreport_UI_HEADERS wrtembed/detailgroupsectiondialog.ui wrtembed/sectioneditor.ui)

add_library(kreport SHARED ${kreport_LIB_SRCS} ${kreport_UI_HEADERS})

qt5_use_modules(kreport Widgets Xml PrintSupport)

target_link_libraries(kreport
  ${KOPROPERTY_LIBS}
  ${CALLIGRADB_LIBS} # ONLY FOR KexiPluginLoader, TODO: remove
  ${KDE4_KROSSCORE_LIBS}
  KF5::WidgetsAddons
  KF5::KrossCore KF5::KrossUi
  KF5::KIOCore KF5::KIOFileWidgets KF5::KIOWidgets KF5::KIONTLM
)

#target_link_libraries(koreport LINK_INTERFACE_LIBRARIES
#  ${KOPROPERTY_LIBS} ${KDE4_KROSSCORE_LIBS} kowidgets)

set_target_properties(kreport PROPERTIES VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION})
install(TARGETS kreport LIBRARY DESTINATION lib)

#TODO
#install(FILES  koreport_itemplugin.desktop  DESTINATION ${SERVICETYPES_INSTALL_DIR})
