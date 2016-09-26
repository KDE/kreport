ecm_create_qm_loader(kreport_barcodeplugin_QM_LOADER kreport_barcodeplugin_qt)

set(kreport_barcodeplugin_LIB_SRCS
    ${PROJECT_SOURCE_DIR}/src/common/kreportplugin_debug.cpp
    KReportItemBarcode.cpp
    KReportDesignerItemBarcode.cpp
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
    KReportBarcodePlugin.cpp
    ${kreport_barcodeplugin_QM_LOADER}
    kreport_barcodeplugin.json
)

if(KREPORT_SCRIPTING)
    list(APPEND kreport_barcodeplugin_LIB_SRCS
        KReportScriptBarcode.cpp
    )
endif()

add_library(org.kde.kreport.barcode MODULE ${kreport_barcodeplugin_LIB_SRCS})

target_link_libraries(org.kde.kreport.barcode
    PUBLIC
        KReport
)

install(TARGETS org.kde.kreport.barcode DESTINATION ${KREPORT_PLUGIN_INSTALL_DIR})

add_subdirectory(pics)
