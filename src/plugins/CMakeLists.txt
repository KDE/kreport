ecm_optional_add_subdirectory(barcode)

if(NOT WIN32)
#todo  ecm_optional_add_subdirectory(chart)
endif(NOT WIN32)

find_package(Marble)
if(MARBLE_FOUND)
  ecm_optional_add_subdirectory(maps)
endif(MARBLE_FOUND)

if(QT_QTWEBKIT_FOUND)
#todo  ecm_optional_add_subdirectory(web)
endif(QT_QTWEBKIT_FOUND)
