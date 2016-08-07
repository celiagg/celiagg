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

set(PYX_DEPS ${CMAKE_CURRENT_SOURCE_DIR}/_pyagg.pyx
             ${CMAKE_CURRENT_SOURCE_DIR}/_agg2d.pxd
             ${CMAKE_CURRENT_SOURCE_DIR}/agg2d.pxi
             ${CMAKE_CURRENT_SOURCE_DIR}/_pyagg.pxd
             ${CMAKE_CURRENT_SOURCE_DIR}/_graphics_state.pxd
             ${CMAKE_CURRENT_SOURCE_DIR}/graphics_state.pxi
             ${CMAKE_CURRENT_SOURCE_DIR}/ndarray_canvas.pxi
             ${CMAKE_CURRENT_SOURCE_DIR}/fast_hist.pxi)
set(CPP_SOURCES ndarray_canvas.cpp)
set(CPP_HEADERS ndarray_canvas.h
                ndarray_canvas.hxx
                graphics_state.h)

set(CYTHON_GENERATED_CPP_FILES "")
# TODO: find cython executable properly
if(APPLE)
    set(CYTHON_EXECUTABLE /usr/local/bin/cython)
else()
    set(CYTHON_EXECUTABLE cython)
endif()

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/_pyagg.cpp
                   COMMAND ${CYTHON_EXECUTABLE} --cplus -3 -I${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/_pyagg.pyx -o ${CMAKE_CURRENT_BINARY_DIR}/_pyagg.cpp
                   DEPENDS ${PYX_DEPS})
list(APPEND CPP_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/pyagg.cpp)

add_library(_pyagg SHARED ${CPP_SOURCES}
                         ${CPP_HEADERS})
target_link_libraries(_pyagg antigrain
                            ${PYTHON_LIBRARIES})
if(AGG2D_USE_FREETYPE)
    target_link_libraries(_pyagg ${FREETYPE_LIBRARIES})
endif()

get_target_property(_PYTHON_MODULE_LOCATION _pyagg LOCATION)
if(WIN32)
    string(REPLACE "/" "\\" _PYTHON_MODULE_LOCATION ${_PYTHON_MODULE_LOCATION})
    string(REPLACE "/" "\\" _PROJECT_SOURCE_DIR ${PROJECT_SOURCE_DIR})
    add_custom_command(TARGET _pyagg
                       POST_BUILD
                       COMMAND copy /Y ${_PYTHON_MODULE_LOCATION} "${_PROJECT_SOURCE_DIR}\\pyagg\\_pyagg.pyd")
    unset(_PROJECT_SOURCE_DIR)
else()
    add_custom_command(TARGET _pyagg
                       POST_BUILD
                       COMMAND cp -fv ${_PYTHON_MODULE_LOCATION} "${PROJECT_SOURCE_DIR}/pyagg/_pyagg.so")
endif()
unset(_PYTHON_MODULE_LOCATION)
