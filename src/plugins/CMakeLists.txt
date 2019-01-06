ecm_optional_add_subdirectory(barcode)

if(NOT WIN32)
#todo  ecm_optional_add_subdirectory(chart)
endif()

find_package(Marble)
if(Marble_FOUND AND NOT WIN32)
  ecm_optional_add_subdirectory(maps)
endif()

find_package(Qt5WebKitWidgets)
if(Qt5WebKitWidgets_FOUND)
  ecm_optional_add_subdirectory(web)
endif()
