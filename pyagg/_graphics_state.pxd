# The MIT License (MIT)
#
# Copyright (c) 2016 WUSTL ZPLAB
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
from libc.stdint cimport uint8_t
from libcpp cimport bool

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

        void antiAliased(bool aa)
        bool antiAliased() const

        void clipBox(Rect r)
        void  clipBox(double x1, double y1, double x2, double y2)
        Rect clipBox() const

        void drawingMode(_enums.DrawingMode m)
        _enums.DrawingMode drawingMode() const

        void blendMode(_enums.BlendMode m)
        _enums.BlendMode blendMode() const

        void imageBlendMode(_enums.BlendMode m)
        _enums.BlendMode imageBlendMode() const

        void masterAlpha(double a)
        double masterAlpha() const

        void antiAliasGamma(double g)
        double antiAliasGamma() const

        void lineWidth(double w)
        double lineWidth() const

        void lineCap(_enums.LineCap cap)
        _enums.LineCap lineCap() const

        void lineJoin(_enums.LineJoin join)
        _enums.LineJoin lineJoin() const

        void stencil(const _image.Image* image)
        const _image.Image* stencil() const
