ecm_create_qm_loader(koreport_barcodeplugin_QM_LOADER kreport_barcodeplugin_qt)

set(kreport_barcodeplugin_LIB_SRCS
    ${PROJECT_SOURCE_DIR}/src/common/kreportplugin_debug.cpp
    KoReportItemBarcode.cpp
    KoReportDesignerItemBarcode.cpp
    krscriptbarcode.cpp
    3of9.cpp
    i2of5.cpp
    code128.cpp
    codeean.cpp
    ext3of9.cpp
    i2of5paint.cpp
    3of9paint.cpp
    ext3of9paint.cpp
    codeeanpaint.cpp
    code128paint.cpp
    KoReportBarcodePlugin.cpp
    ${koreport_barcodeplugin_QM_LOADER}
)

add_library(kreport_barcodeplugin MODULE ${kreport_barcodeplugin_LIB_SRCS})
kcoreaddons_desktop_to_json(kreport_barcodeplugin kreport_barcodeplugin.desktop)

target_link_libraries(kreport_barcodeplugin
    PUBLIC
        KReport
)

install(TARGETS kreport_barcodeplugin DESTINATION ${KREPORT_PLUGIN_INSTALL_DIR})
#ecm_install_icons(${DATA_INSTALL_DIR}/calligra/icons)
