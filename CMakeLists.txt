# The MIT License (MIT)
#
# Copyright (c) 2014 WUSTL ZPLAB
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
# Authors: Erik Hvatum <ice.rikh@gmail.com>

# CMake 2.8.11 introduced the target_link_libraries command, a huge time saver
cmake_minimum_required(VERSION 2.8.11)

project(pyagg)
set(MAJOR_VERSION 0)
set(MINOR_VERSION 1)
set(PACKAGE pyagg)
set(PACKAGE_VERSION ${MAJOR_VERSION}.${MINOR_VERSION})

set(CMAKE_INCLUDE_CURRENT_DIR ON CACHE BOOL "Find includes in corresponding build directories")
set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "")
if(NOT WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
endif()

set(Python_ADDITIONAL_VERSIONS ${Python_ADDITIONAL_VERSIONS} "3.4")
include_directories(SYSTEM /usr/local/include)
find_package(PythonInterp 3.4 REQUIRED)
if(NOT APPLE)
    find_package(PythonLibs 3.4 REQUIRED)
else()
    # FindPythonLibs.cmake is totally horked on OS X and will probably remain so until the end of time and beyond: http://www.itk.org/Bug/view.php?id=14809
    # Because Find*.cmake modules must support cross compilation, the obvious fix of just asking the interpreter where its stuff is can't go into the
    # official CMake distribution.  Because it's better to have it not work at all under any circumstance, I guess?  Okay, that would be fine, except
    # we need this to work.  So, we resort to asking the interpreter.
    execute_process(COMMAND ${PYTHON_EXECUTABLE} -c "import distutils.sysconfig; vs = distutils.sysconfig.get_config_vars(); print('LIBPL:\"{}\"LIBRARY:\"{}\"get_python_lib:\"{}\"get_python_inc:\"{}\"'.format(vs['LIBPL'], vs['LIBRARY'], distutils.sysconfig.get_python_lib(), distutils.sysconfig.get_python_inc()))"
                    RESULT_VARIABLE _PYC_RESULT
                    ERROR_VARIABLE _PYC_ERROR
                    OUTPUT_VARIABLE _PYC_OUT
                    OUTPUT_STRIP_TRAILING_WHITESPACE)
    if(NOT _PYC_RESULT MATCHES 0)
        message(FATAL_ERROR "Failed to interrogate Python interpreter (which must be done in order to locate the CPython library and header directories):\n${_PYC_ERROR}")
        return()
    endif()
    string(REGEX REPLACE
           "LIBPL:\"(.+)\"LIBRARY:\"(.+)\"get_python_lib:\"(.+)\"get_python_inc:\"(.+)\""
           "\\1;\\2;\\3;\\4"
           _PYC_OUTS
           ${_PYC_OUT})
    set(PYTHONLIBS_FOUND true)
    list(GET _PYC_OUTS 0 _PYTHON_LIB_DIR)
    list(GET _PYC_OUTS 1 _PYTHON_LIB_NAME)
    set(PYTHON_LIBRARIES "${_PYTHON_LIB_DIR}/${_PYTHON_LIB_NAME}")
    set(PYTHON_LIBRARY ${PYTHON_LIBRARIES})
    list(GET _PYC_OUTS 3 PYTHON_INCLUDE_PATH)
    list(GET _PYC_OUTS 3 PYTHON_INCLUDE_DIRS)
    unset(_PYC_RESULT)
    unset(_PYC_ERROR)
    unset(_PYC_OUT)
    unset(_PYC_OUTS)
    unset(_PYTHON_LIB_DIR)
    unset(_PYTHON_LIB_NAME)
    message(STATUS "Found PythonLibs: ${PYTHON_LIBRARY} (do not be alarmed if the filename ends in .a; dynamic linking will be used nonetheless, if possible)")
endif()
execute_process(COMMAND ${PYTHON_EXECUTABLE} -c "import numpy;print(numpy.get_include())"
                RESULT_VARIABLE _NUMPY_INCLUDE_DIR_FOUND
                ERROR_VARIABLE _NUMPY_INCLUDE_DIR_ERROR
                OUTPUT_VARIABLE NUMPY_INCLUDE_DIR
                OUTPUT_STRIP_TRAILING_WHITESPACE)
if(NOT _NUMPY_INCLUDE_DIR_FOUND MATCHES 0)
    message(FATAL_ERROR "Failed to import numpy (which must be done in order to locate the numpy C API header directory):\n${_NUMPY_INCLUDE_DIR_ERROR}")
    return()
endif()
unset(_NUMPY_INCLUDE_DIR_FOUND)
unset(_NUMPY_INCLUDE_DIR_ERROR)

include_directories(SYSTEM ${PYTHON_INCLUDE_DIRS}
                           ${NUMPY_INCLUDE_DIR})
include_directories(USER ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include
                         ${PROJECT_SOURCE_DIR}/agg-svn/agg-2.4/include/ctrl)

add_subdirectory(agg-svn/agg-2.4/src)
add_subdirectory(cython)
