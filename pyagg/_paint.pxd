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

cimport _enums


cdef extern from "paint.h":
    cdef cppclass GradientStop:
        double off
        double r
        double g
        double b
        double a

        GradientStop(const double o, const double r, const double g,
                     const double b, const double a)

    cdef cppclass Paint:
        Paint(const double r, const double g, const double b,
              const double a)
        Paint(const _enums.PaintType type,
              double* points, const unsigned n_points,
              double* stops, const unsigned n_stops,
              const _enums.GradientSpread spread,
              const _enums.GradientUnits units)
        Paint(const _enums.PatternStyle style,
              unsigned char* buf, unsigned width, unsigned height, int stride)

        double  a()
        double  r()
        double  g()
        double  b()
        void  a(const double _a)
        void  r(const double _r)
        void  g(const double _g)
        void  b(const double _b)
