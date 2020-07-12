# This file defines the KDB_CREATE_SHARED_DATA_CLASSES macro.
#
# KDB_CREATE_SHARED_DATA_CLASSES(LIST)
#   Creates header file out of the LIST files in .shared.h format.
#   FILES should be a list of relative paths to ${CMAKE_SOURCE_DIR}.
#
# Following example creates ${CMAKE_CURRENT_BINARY_DIR}/Foo.h
# and ${CMAKE_CURRENT_BINARY_DIR}/Baz.h files:
#
# INCLUDE(KDbCreateSharedDataClass)
# KDB_CREATE_SHARED_DATA_CLASS(OUTPUT_VAR Bar/Foo.shared.h Baz.shared.h)
#
# Copyright (C) 2010 Jarosław Staniek <staniek@kde.org>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

macro(KDB_CREATE_SHARED_DATA_CLASSES)
    # message(STATUS "KDB_CREATE_SHARED_DATA_CLASSES ARGV: ${ARGV}")
    set(_args "")
    list(APPEND _args ${ARGV})
    list(GET _args 0 OUTPUT_VAR)
    list(GET _args 1 PREFIX)
    list(REMOVE_AT _args 0 1)
    # message(STATUS "OUTPUT_VAR: ${OUTPUT_VAR} ${_args}")
    find_package(PythonInterp)
    set_package_properties(PythonInterp PROPERTIES DESCRIPTION "Python language interpreter"
                           URL "https://www.python.org" TYPE REQUIRED
                           PURPOSE "Required by the Shared Data Classes (SDC) tool")
    foreach(_input ${_args})
        get_filename_component(INPUT ${_input} ABSOLUTE)
        string(REGEX REPLACE "\\.shared\\.h" ".h" OUTPUT ${_input})
        string(REGEX REPLACE ".*/([^/]+)\\.h" "\\1.h" OUTPUT ${OUTPUT})
        #MESSAGE(DEBUG "--------- ${_input} ${OUTPUT} ${INPUT}")
        #MESSAGE(DEBUG "COMMAND python2 ${CMAKE_SOURCE_DIR}/tools/sdc.py ${INPUT} ${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT}")
        message(STATUS "Creating shared data class in ${OUTPUT} from ${_input}")
        if(${PREFIX} STREQUAL "NO_PREFIX")
            set(OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR})
        else()
            set(OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/${PREFIX})
            set(OUTPUT "${PREFIX}/${OUTPUT}")
        endif()
        # message(STATUS "OUTPUT_DIR: ${OUTPUT_DIR} ${PREFIX}")
        file(MAKE_DIRECTORY ${OUTPUT_DIR})
        execute_process(
            COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/sdc.py
                           ${INPUT}
                           ${OUTPUT_DIR}/${OUTPUT}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            RESULT_VARIABLE KDB_CREATE_SHARED_DATA_CLASSES_RESULT
        )
        # MESSAGE(STATUS "...result: ${KDB_CREATE_SHARED_DATA_CLASSES_RESULT}")
        list(APPEND ${OUTPUT_VAR} "${OUTPUT}")
    endforeach(_input)
endmacro(KDB_CREATE_SHARED_DATA_CLASSES)

macro(KDB_REMOVE_EXTENSIONS)
    # message(STATUS "KDB_REMOVE_EXTENSIONS ARGV: ${ARGV}")
    set(_args "")
    list(APPEND _args ${ARGV})
    list(GET _args 0 OUTPUT_VAR)
    list(REMOVE_AT _args 0)
    # message(STATUS "OUTPUT_VAR: ${OUTPUT_VAR} ${_args}")
    foreach(_input ${_args})
        string(REGEX REPLACE "\\.h" "" OUTPUT ${_input})
        # message(STATUS "...result: ${OUTPUT}")
        list(APPEND ${OUTPUT_VAR} "${OUTPUT}")
    endforeach(_input)
endmacro(KDB_REMOVE_EXTENSIONS)
