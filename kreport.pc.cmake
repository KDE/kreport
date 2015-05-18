prefix=@CMAKE_INSTALL_PREFIX@
exec_prefix=${prefix}
libdir=${exec_prefix}/lib
includedir=${prefix}/include

Name: KReport
Description: A framework for the creation and generation of reports in multiple formats
Version: @KREPORT_VERSION@
URL: https://projects.kde.org/kreport
Requires: Qt5Core Qt5Gui Qt5Widgets Qt5Xml Qt5PrintSupport KF5CoreAddons KF5WidgetsAddons KF5WidgetsAddons KProperty
Libs: -L${libdir}
Cflags: -I${includedir}
