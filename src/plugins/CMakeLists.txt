ecm_optional_add_subdirectory(barcode)

if(NOT WIN32)
#todo  ecm_optional_add_subdirectory(chart)
endif()

find_package(Marble)
if(Marble_FOUND)
  ecm_optional_add_subdirectory(maps)
endif()

if(QT_QTWEBKIT_FOUND)
#todo  ecm_optional_add_subdirectory(web)
endif()
