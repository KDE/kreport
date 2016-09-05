prefix=@CMAKE_INSTALL_PREFIX@
exec_prefix=${prefix}
libdir=${exec_prefix}/lib
includedir=${prefix}/include

Name: KReport
Description: A framework for the creation and generation of reports in multiple formats
Version: @PROJECT_VERSION@
URL: https://community.kde.org/KReport
Requires: Qt5Core Qt5Gui Qt5Widgets Qt5Xml Qt5PrintSupport KF5CoreAddons KF5WidgetsAddons KF5GuiAddons KPropertyCore KPropertyWidgets
Libs: -L${libdir}
Cflags: -I${includedir}
