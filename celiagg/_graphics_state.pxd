# The MIT License (MIT)
#
# Copyright (c) 2016 WUSTL ZPLAB
# Copyright (c) 2016-2021 Celiagg Contributors
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
# Authors: John Wiggins

import cython
cimport numpy
import numpy
from libcpp cimport bool
from libcpp.vector cimport vector

cimport _enums
cimport _image


cdef extern from "graphics_state.h" namespace "GraphicsState":
    cdef cppclass Rect:
        double x1
        double y1
        double x2
        double y2
        Rect()
        Rect(double x1, double y1, double x2, double y2)
        bool is_valid() const


cdef extern from "graphics_state.h":
    cdef cppclass GraphicsState:
        GraphicsState()

        void anti_aliased(bool aa)
        bool anti_aliased() const

        void clip_box(Rect r)
        Rect clip_box() const

        void drawing_mode(_enums.DrawingMode m)
        _enums.DrawingMode drawing_mode() const

        void text_drawing_mode(_enums.TextDrawingMode m)
        _enums.TextDrawingMode text_drawing_mode() const

        void blend_mode(_enums.BlendMode m)
        _enums.BlendMode blend_mode() const

        void image_blend_mode(_enums.BlendMode m)
        _enums.BlendMode image_blend_mode() const

        void master_alpha(double a)
        double master_alpha() const

        void line_width(double w)
        double line_width() const

        void line_cap(_enums.LineCap cap)
        _enums.LineCap line_cap() const

        void line_join(_enums.LineJoin join)
        _enums.LineJoin line_join() const

        void inner_join(_enums.InnerJoin join)
        _enums.InnerJoin inner_join() const

        void miter_limit(double limit)
        double miter_limit() const

        void inner_miter_limit(double limit)
        double inner_miter_limit() const

        void line_dash_pattern(const double *dashes, size_t count)
        const vector[double]& line_dash_pattern() const

        void line_dash_phase(const double phase)
        double line_dash_phase() const

        void stencil(const _image.Image* image)
        const _image.Image* stencil() const
