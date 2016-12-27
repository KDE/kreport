find_package(Qt5Widgets REQUIRED)
find_package(KF5WidgetsAddons)
find_package(KF5GuiAddons)

remove_definitions(
    -DQT_NO_KEYWORDS
    -DQT_NO_SIGNALS_SLOTS_KEYWORDS
    -DQT_NO_CAST_FROM_ASCII
)

add_definitions(-DKREPORTEXAMPLE_DATA_DIR="\\"${CMAKE_CURRENT_SOURCE_DIR}\\"")

set(kreportexample_SRCS main.cpp
        window.cpp
        DesignerWidget.cpp
        KReportExampleData.cpp
)
add_executable(kreportexample ${kreportexample_SRCS})
target_link_libraries(kreportexample KF5::ConfigGui KReport KReportUtilsPrivate)
