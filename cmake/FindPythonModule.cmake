#
# This file was originally written for GROMACS, but
# just as the other CMake configuration files you can
# redistributed it freely under the BSD 3-clause license.
#
# Adapted from code posted on cmake-users by Mark Moll (the execute_process()
# call remains, but other things have been rewritten for nicer behavior).
#

find_package(PythonInterp 2.7)

function (find_python_module module)
    string(TOUPPER ${module} _module_upper)
    set(_find_package_module ${module})
    set(_out_var PYTHONMODULE_${_module_upper})

    include(CMakeParseArguments)
    set(_options QUIET REQUIRED)
    cmake_parse_arguments(ARG "${_options}" "" "" ${ARGN})
    if (ARG_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "Unknown arguments: ${ARG_UNPARSED_ARGUMENTS}")
    endif()
    if (ARG_REQUIRED)
        set(${_find_package_module}_FIND_REQUIRED TRUE)
    endif()
    if (ARG_QUIET)
        set(${_find_package_module}_FIND_QUIETLY TRUE)
    endif()

    if (NOT ${_out_var})
        set(_status 1)
        if (PYTHON_EXECUTABLE)
            # A module's location is usually a directory, but for binary modules
            # it's a .so file.
            execute_process(COMMAND "${PYTHON_EXECUTABLE}" "-c"
                "import re, ${module}; print re.compile('/__init__.py.*').sub('',${module}.__file__)"
                RESULT_VARIABLE _status
                OUTPUT_VARIABLE _location
                ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
        endif()
        if(_status)
            set(_location ${_find_package_module}-NOTFOUND)
        endif()
        set(${_out_var} ${_location} CACHE STRING
            "Location of Python module ${module}" FORCE)
        mark_as_advanced(${_out_var})
    endif()
    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(
        ${_find_package_module} DEFAULT_MSG
        ${_out_var} PYTHON_EXECUTABLE)
endfunction()
