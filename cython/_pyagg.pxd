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

import cython
cimport numpy
import numpy
from libc.stdint cimport uint8_t
from libcpp cimport bool

cdef extern from "ndarray_canvas.h" namespace "agg":
    cdef cppclass pixfmt_rgb24:
        pass
    cdef cppclass pixfmt_rgba32:
        pass
    cdef cppclass pixfmt_gray8:
        pass
    cdef cppclass pixfmt_rgb48:
        pass
    cdef cppclass pixfmt_rgba64:
        pass
    cdef cppclass pixfmt_gray16:
        pass

cdef extern from "ndarray_canvas.h":
    cdef cppclass pixfmt_gray8_no_alpha:
        pass

    cdef cppclass ndarray_canvas_base[value_type_T]:
        const size_t& channel_count() const
        unsigned width() const
        unsigned height() const
        void draw_line(const double& x0, const double& y0,
                       const double& x1, const double& y1,
                       const double& w,
                       const value_type_T* c,
                       const bool& aa)
        void draw_polygon(const double* points, const size_t& point_count,
                          const bool& outline, const double& outline_w, const value_type_T* outline_c,
                          const bool& fill, const value_type_T* fill_c,
                          const bool& aa)
        void draw_bezier3(const double& x0, const double& y0,
                          const double& x_ctrl, const double& y_ctrl,
                          const double& x1, const double& y1,
                          const double& w, const value_type_T* c,
                          const bool& aa)
        void draw_bezier3_composite(const double* points, const size_t& point_count,
                                    const double& w, const value_type_T* c,
                                    const bool& aa)
        void draw_bezier4(const double& x0, const double& y0,
                          const double& x_ctrl0, const double& y_ctrl0,
                          const double& x_ctrl1, const double& y_ctrl1,
                          const double& x1, const double& y1,
                          const double& w, const value_type_T* c,
                          const bool& aa)
        void draw_bezier4_composite(const double* points, const size_t& point_count,
                                    const double& w, const value_type_T* c,
                                    const bool& aa)

    cdef cppclass ndarray_canvas[pixfmt_T, value_type_T]:
        ndarray_canvas(value_type_T* buf, const unsigned& width, const unsigned& height, const int& stride, const size_t& channel_count)
        const size_t& channel_count() const
        unsigned width() const
        unsigned height() const
        void draw_line(const double& x0, const double& y0,
                       const double& x1, const double& y1,
                       const double& w,
                       const value_type_T* c,
                       const bool& aa)
        void draw_polygon(const double* points, const size_t& point_count,
                          const bool& outline, const double& outline_w, const uint8_t* outline_c,
                          const bool& fill, const value_type_T* fill_c,
                          const bool& aa)
        void draw_bezier3(const double& x0, const double& y0,
                          const double& x_ctrl, const double& y_ctrl,
                          const double& x1, const double& y1,
                          const double& w, const value_type_T* c,
                          const bool& aa)
        void draw_bezier3_composite(const double* points, const size_t& point_count,
                                    const double& w, const value_type_T* c,
                                    const bool& aa)
        void draw_bezier4(const double& x0, const double& y0,
                          const double& x_ctrl0, const double& y_ctrl0,
                          const double& x_ctrl1, const double& y_ctrl1,
                          const double& x1, const double& y1,
                          const double& w, const value_type_T* c,
                          const bool& aa)
        void draw_bezier4_composite(const double* points, const size_t& point_count,
                                    const double& w, const value_type_T* c,
                                    const bool& aa)
