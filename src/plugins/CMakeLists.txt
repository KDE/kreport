ecm_optional_add_subdirectory(barcode)

if(NOT WIN32)
#todo  ecm_optional_add_subdirectory(chart)
endif()

#TODO: enable when Marble is stable and useful for KF5/Qt5
#find_package(Marble)
if(MARBLE_FOUND)
  ecm_optional_add_subdirectory(maps)
endif()

if(QT_QTWEBKIT_FOUND)
#todo  ecm_optional_add_subdirectory(web)
endif()
