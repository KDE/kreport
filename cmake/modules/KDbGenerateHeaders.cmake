#.rst:
# ECMGenerateHeaders
# ------------------
#
# Generate C/C++ CamelCase forwarding headers.
#
# ::
#
#   ecm_generate_headers(<camelcase_forwarding_headers_var>
#       HEADER_NAMES <CamelCaseName> [<CamelCaseName> [...]]
#       [ORIGINAL <CAMELCASE|LOWERCASE>]
#       [OUTPUT_DIR <output_dir>]
#       [PREFIX <prefix>]
#       [REQUIRED_HEADERS <variable>]
#       [RELATIVE <relative_path>])
#
# For each CamelCase header name passed to HEADER_NAMES, a file of that name
# will be generated that will include a version with ``.h`` appended.
# For example, the generated header ``ClassA`` will include ``classa.h`` (or
# ``ClassA.h``, see ORIGINAL).
# The file locations of these generated headers will be stored in
# <camelcase_forwarding_headers_var>.
#
# ORIGINAL specifies how the name of the original header is written: lowercased
# or also camelcased.  The default is LOWERCASE. Since 1.8.0.
#
# PREFIX places the generated headers in subdirectories.  This should be a
# CamelCase name like ``KParts``, which will cause the CamelCase forwarding
# headers to be placed in the ``KParts`` directory (e.g. ``KParts/Part``).  It
# will also, for the convenience of code in the source distribution, generate
# forwarding headers based on the original names (e.g. ``kparts/part.h``).  This
# allows includes like ``"#include <kparts/part.h>"`` to be used before
# installation, as long as the include_directories are set appropriately.
#
# OUTPUT_DIR specifies where the files will be generated; this should be within
# the build directory. By default, ``${CMAKE_CURRENT_BINARY_DIR}`` will be used.
# This option can be used to avoid file conflicts.
#
# REQUIRED_HEADERS specifies an output variable name where all the required
# headers will be appended so that they can be installed together with the
# generated ones.  This is mostly intended as a convenience so that adding a new
# header to a project only requires specifying the CamelCase variant in the
# CMakeLists.txt file; the original variant will then be added to this
# variable.
#
# The RELATIVE argument indicates where the original headers can be found
# relative to CMAKE_CURRENT_SOURCE_DIR.  It does not affect the generated
# CamelCase forwarding files, but ecm_generate_headers() uses it when checking
# that the original header exists, and to generate originally named forwarding
# headers when PREFIX is set.
#
# To allow other parts of the source distribution (eg: tests) to use the
# generated headers before installation, it may be desirable to set the
# INCLUDE_DIRECTORIES property for the library target to output_dir.  For
# example, if OUTPUT_DIR is CMAKE_CURRENT_BINARY_DIR (the default), you could do
#
# .. code-block:: cmake
#
#   target_include_directories(MyLib PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>")
#
# Example usage (without PREFIX):
#
# .. code-block:: cmake
#
#   ecm_generate_headers(
#       MyLib_FORWARDING_HEADERS
#       HEADERS
#           MLFoo
#           MLBar
#           # etc
#       REQUIRED_HEADERS MyLib_HEADERS
#   )
#   install(FILES ${MyLib_FORWARDING_HEADERS} ${MyLib_HEADERS}
#           DESTINATION ${CMAKE_INSTALL_PREFIX}/include
#           COMPONENT Devel)
#
# Example usage (with PREFIX):
#
# .. code-block:: cmake
#
#   ecm_generate_headers(
#       MyLib_FORWARDING_HEADERS
#       HEADERS
#           Foo
#           Bar
#           # etc
#       PREFIX MyLib
#       REQUIRED_HEADERS MyLib_HEADERS
#   )
#   install(FILES ${MyLib_FORWARDING_HEADERS}
#           DESTINATION ${CMAKE_INSTALL_PREFIX}/include/MyLib
#           COMPONENT Devel)
#   install(FILES ${MyLib_HEADERS}
#           DESTINATION ${CMAKE_INSTALL_PREFIX}/include/mylib
#           COMPONENT Devel)
#
# Since pre-1.0.0.

#=============================================================================
# Copyright 2013 Aleix Pol Gonzalez <aleixpol@blue-systems.com>
# Copyright 2014 Alex Merry <alex.merry@kdemail.net>
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file COPYING-CMAKE-SCRIPTS for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of extra-cmake-modules, substitute the full
#  License text for the above reference.)

include(CMakeParseArguments)

function(ECM_GENERATE_HEADERS camelcase_forwarding_headers_var)
    set(options)
    set(oneValueArgs ORIGINAL OUTPUT_DIR PREFIX REQUIRED_HEADERS RELATIVE SOURCE_DIR)
    set(multiValueArgs HEADER_NAMES)
    cmake_parse_arguments(EGH "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (EGH_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "Unexpected arguments to ECM_GENERATE_HEADERS: ${EGH_UNPARSED_ARGUMENTS}")
    endif()

    if(NOT EGH_HEADER_NAMES)
       message(FATAL_ERROR "Missing header_names argument to ECM_GENERATE_HEADERS")
    endif()

    if(NOT EGH_ORIGINAL)
        # default
        set(EGH_ORIGINAL "LOWERCASE")
    endif()
    if(NOT EGH_ORIGINAL STREQUAL "LOWERCASE" AND NOT EGH_ORIGINAL STREQUAL "CAMELCASE")
        message(FATAL_ERROR "Unexpected value for original argument to ECM_GENERATE_HEADERS: ${EGH_ORIGINAL}")
    endif()

    if(NOT EGH_OUTPUT_DIR)
        set(EGH_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}")
    endif()

    # Make sure EGH_RELATIVE is /-terminated when it's not empty
    if (EGH_RELATIVE AND NOT "${EGH_RELATIVE}" MATCHES "^.*/$")
        set(EGH_RELATIVE "${EGH_RELATIVE}/")
    endif()

    if (EGH_PREFIX)
        if (NOT "${EGH_PREFIX}" MATCHES "^.*/$")
            set(EGH_PREFIX "${EGH_PREFIX}/")
        endif()
        if (EGH_ORIGINAL STREQUAL "CAMELCASE")
            set(originalprefix "${EGH_PREFIX}")
        else()
            string(TOLOWER "${EGH_PREFIX}" originalprefix)
        endif()
    endif()

    foreach(_CLASSNAME ${EGH_HEADER_NAMES})
        if (EGH_ORIGINAL STREQUAL "CAMELCASE")
            set(originalclassname "${_CLASSNAME}")
        else()
            string(TOLOWER "${_CLASSNAME}" originalclassname)
        endif()
        set(FANCY_HEADER_FILE "${EGH_OUTPUT_DIR}/${EGH_PREFIX}${_CLASSNAME}")
        if (EGH_SOURCE_DIR)
            set(_actualheader "${EGH_SOURCE_DIR}/${EGH_RELATIVE}${originalclassname}.h")
        else()
            set(_actualheader "${CMAKE_CURRENT_SOURCE_DIR}/${EGH_RELATIVE}${originalclassname}.h")
        endif()
        if (NOT EXISTS ${_actualheader})
            message(FATAL_ERROR "Could not find \"${_actualheader}\"")
        endif()
        if (NOT EXISTS ${FANCY_HEADER_FILE})
            file(WRITE ${FANCY_HEADER_FILE} "#include \"${originalprefix}${originalclassname}.h\"\n")
        endif()
        list(APPEND ${camelcase_forwarding_headers_var} "${FANCY_HEADER_FILE}")
        if (EGH_REQUIRED_HEADERS)
            list(APPEND ${EGH_REQUIRED_HEADERS} "${_actualheader}")
        endif()
        if (EGH_PREFIX)
            # Local forwarding header, for namespaced headers, e.g. kparts/part.h
            set(REGULAR_HEADER_NAME ${EGH_OUTPUT_DIR}/${originalprefix}${originalclassname}.h)
            if (NOT EXISTS ${REGULAR_HEADER_NAME})
                file(WRITE ${REGULAR_HEADER_NAME} "#include \"${_actualheader}\"\n")
            endif()
        endif()
    endforeach()

    set(${camelcase_forwarding_headers_var} ${${camelcase_forwarding_headers_var}} PARENT_SCOPE)
    if (NOT EGH_REQUIRED_HEADERS STREQUAL "")
        set(${EGH_REQUIRED_HEADERS} ${${EGH_REQUIRED_HEADERS}} PARENT_SCOPE)
    endif ()
endfunction()
