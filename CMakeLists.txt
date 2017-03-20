cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
find_package(ECM 1.8.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})
include(SetKReportCMakePolicies NO_POLICY_SCOPE)

project(KReport VERSION 3.0.2) # Update this

# CMake
include(CMakePackageConfigHelpers)

# ECM
include(ECMGeneratePriFile)
include(ECMPoQmTools)
include(ECMSetupVersion)
include(ECMInstallIcons)
include(ECMOptionalAddSubdirectory)
include(KDEInstallDirs)
include(KDECMakeSettings NO_POLICY_SCOPE)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)

# Own
include(KReportAddIconsRccFile)
include(KReportMacros)

# Dependencies
set(REQUIRED_QT_VERSION 5.4.0)
set(REQUIRED_KF5_VERSION 5.16.0)
set(REQUIRED_KPROPERTY_VERSION 3.0)
set(MAXIMUM_KPROPERTY_VERSION 3.0.80) # .80 means < 3.1 Alpha

# Required components to build this framework
# TODO move Widgets/KF5WidgetsAddons-dependent part to a libKReportDesigner
find_package(Qt5 ${REQUIRED_QT_VERSION} NO_MODULE REQUIRED Core Widgets Xml PrintSupport)
find_package(KF5 ${REQUIRED_KF5_VERSION} REQUIRED CoreAddons WidgetsAddons GuiAddons Config)
find_package(KPropertyWidgets ${REQUIRED_KPROPERTY_VERSION} REQUIRED)
check_maximum_package_version(KPropertyWidgets ${MAXIMUM_KPROPERTY_VERSION})

get_git_revision_and_branch()
add_tests()
add_examples()
add_unfinished_features_option()
add_pc_file(${PROJECT_NAME})

add_subdirectory(src)

if (IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/po")
    ecm_install_po_files_as_qm(po)
endif()

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
macro_display_feature_log()
