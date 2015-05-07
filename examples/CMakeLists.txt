find_package(Qt5Widgets REQUIRED)
find_package(KF5WidgetsAddons)
find_package(KF5GuiAddons)

remove_definitions(
    -DQT_NO_KEYWORDS
    -DQT_NO_SIGNALS_SLOTS_KEYWORDS
    -DQT_NO_CAST_FROM_ASCII
)

set(kreportexample_SRCS main.cpp window.cpp)
add_executable(kreportexample ${kreportexample_SRCS})
target_link_libraries(kreportexample Qt5::Widgets KReport)
target_include_directories(kreportexample PUBLIC ${CMAKE_BINARY_DIR}/src/kreport)
