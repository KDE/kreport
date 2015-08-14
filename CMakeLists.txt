cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)
project(KReport)

# ECM setup
find_package(ECM 1.8.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

if(POLICY CMP0063) # Honor visibility properties for all target types (since cmake 3.3)
    cmake_policy(SET CMP0063 OLD)
endif()

include(ECMGenerateHeaders)
include(ECMGeneratePriFile)
include(ECMPackageConfigHelpers)
include(ECMPoQmTools)
include(ECMSetupVersion)
include(ECMInstallIcons)
include(ECMOptionalAddSubdirectory)

include(KDEInstallDirs)
include(KDEFrameworkCompilerSettings)
include(KDECMakeSettings)

include(FeatureSummary)
include(GenerateExportHeader)

set(KREPORT_VERSION "2.96.0") # Update this

# Dependencies
set(REQUIRED_QT_VERSION "5.2.0")

# Required components to build this framework
# TODO move Widgets/KF5WidgetsAddons-dependent part to a libKReportDesigner
find_package(Qt5 ${REQUIRED_QT_VERSION} NO_MODULE REQUIRED Core Widgets Xml PrintSupport)
find_package(KF5 5.7.0 REQUIRED CoreAddons WidgetsAddons GuiAddons Config)
find_package(KProperty 2.96.0 REQUIRED)

set(KREPORT_PLUGIN_INSTALL_DIR ${PLUGIN_INSTALL_DIR}/kreport)

if(BUILD_TESTING)
    add_definitions(-DBUILD_TESTING)
    add_subdirectory( autotests )
    #add_subdirectory( tests )
endif()

option(BUILD_EXAMPLES "Build example applications" FALSE)
if (BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

add_subdirectory(src)

if (NOT WIN32)
  configure_file(${KReport_SOURCE_DIR}/KReport.pc.cmake ${KReport_BINARY_DIR}/KReport.pc @ONLY)
  install(FILES ${KReport_BINARY_DIR}/KReport.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig)
endif ()

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
