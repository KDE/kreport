cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
find_package(ECM 5.45.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})
include(SetKReportCMakePolicies NO_POLICY_SCOPE)

project(KReport VERSION 3.2.90) # Update this: Alpha = 90, Beta 1 = 91, RC 1 = 94

include(KReportAddTests)
include(KReportAddExamples)
kreport_add_tests(OFF)
kreport_add_examples(OFF)

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
include(ECMSetupQtPluginMacroNames)

# Own
include(KReportAddIconsRccFile)
include(KReportAddQCH)
include(KReportMacros)

simple_option(BUILD_QCH "Build API documentation in QCH format" OFF)

# Dependencies
set(REQUIRED_QT_VERSION 5.4.0)
set(REQUIRED_KF5_VERSION 5.16.0)
set(REQUIRED_KPROPERTY_VERSION ${PROJECT_VERSION})

# Required components to build this framework
# TODO move Widgets/KF5WidgetsAddons-dependent part to a libKReportDesigner
find_package(Qt5 ${REQUIRED_QT_VERSION} NO_MODULE REQUIRED Core Widgets Xml PrintSupport)
find_package(KF5 ${REQUIRED_KF5_VERSION} REQUIRED CoreAddons WidgetsAddons GuiAddons Config)
find_package(KPropertyWidgets ${REQUIRED_KPROPERTY_VERSION} COMPONENTS KF)
set_package_properties(KPropertyWidgets PROPERTIES TYPE REQUIRED
                       PURPOSE "Required by KReport for handling properties")

get_git_revision_and_branch()
add_unfinished_features_option()
add_pc_file(${PROJECT_NAME})

ecm_setup_qtplugin_macro_names(
    JSON_ARG2 KREPORT_PLUGIN_FACTORY
    CONFIG_CODE_VARIABLE KREPORT_SETUP_AUTOMOC_VARIABLES
)

add_subdirectory(src)

if(BUILD_TESTING)
  add_subdirectory(autotests)
  #add_subdirectory(tests)
endif()

if(BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

if (IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/po")
    ecm_install_po_files_as_qm(po)
endif()

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
