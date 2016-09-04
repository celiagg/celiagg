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
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
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

cimport _graphics_state
cimport _paint
cimport _path
cimport _text
cimport _transform


cdef extern from "ndarray_canvas.h" namespace "agg":
    cdef cppclass pixfmt_rgba32:
        pass
    cdef cppclass pixfmt_rgb24:
        pass
    cdef cppclass pixfmt_gray16:
        pass
    cdef cppclass pixfmt_gray8:
        pass
    cdef cppclass pixfmt_rgb48:
        pass
    cdef cppclass pixfmt_rgba64:
        pass

cdef extern from "image.h":
    cdef cppclass Image:
        Image(uint8_t* buf, unsigned width, unsigned height, int stride)
        int width()
        int height()

cdef extern from "ndarray_canvas.h":
    cdef cppclass ndarray_canvas_base:
        const size_t& channel_count() const
        unsigned width() const
        unsigned height() const
        void draw_path(const _path.path_storage& path,
                       const _transform.trans_affine& transform,
                       _paint.Paint& linePaint, _paint.Paint& fillPaint,
                       const _graphics_state.GraphicsState& gs)
        void draw_bspline(const double* points, const size_t& point_count,
                          const _transform.trans_affine& transform,
                          _paint.Paint& linePaint, _paint.Paint& fillPaint,
                          const _graphics_state.GraphicsState& gs)
        void draw_image(Image& img, const _transform.trans_affine& transform,
                        const _graphics_state.GraphicsState& gs)
        void draw_text(const char* text, _text.Font& font,
                       const _transform.trans_affine& transform,
                       _paint.Paint& linePaint, _paint.Paint& fillPaint,
                       const _graphics_state.GraphicsState& gs)

    cdef cppclass ndarray_canvas[pixfmt_T]:
        ndarray_canvas(uint8_t* buf,
                       const unsigned& width,
                       const unsigned& height,
                       const int& stride,
                       const size_t& channel_count)
        const size_t& channel_count() const
        unsigned width() const
        unsigned height() const
        void draw_path(const _path.path_storage& path,
                       const _transform.trans_affine& transform,
                       _paint.Paint& linePaint, _paint.Paint& fillPaint,
                       const _graphics_state.GraphicsState& gs)
        void draw_bspline(const double* points, const size_t& point_count,
                          const _transform.trans_affine& transform,
                          _paint.Paint& linePaint, _paint.Paint& fillPaint,
                          const _graphics_state.GraphicsState& gs)
        void draw_image(Image& img, const _transform.trans_affine& transform,
                        const _graphics_state.GraphicsState& gs)
        void draw_text(const char* text, _text.Font& font,
                       const _transform.trans_affine& transform,
                       _paint.Paint& linePaint, _paint.Paint& fillPaint,
                       const _graphics_state.GraphicsState& gs)
