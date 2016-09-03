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

cpdef enum PaintType:
    Solid = _enums.k_PaintTypeSolid
    Linear = _enums.k_PaintTypeLinearGradient
    Radial = _enums.k_PaintTypeRadialGradient

cpdef enum GradientSpread:
    Pad = _enums.k_GradientSpreadPad
    Reflect = _enums.k_GradientSpreadReflect
    Repeat = _enums.k_GradientSpreadRepeat

cpdef enum GradientUnits:
    UserSpace = _enums.k_GradientUnitsUserSpace
    ObjectBoundingBox = _enums.k_GradientUnitsObjectBoundingBox


cdef _get_gradient_points(points, length):
    cdef double[::1] arr = numpy.asarray(points,
                                         dtype=numpy.float64,
                                         order='c')
    if arr.shape[0] != length:
        msg = 'points argument must be an iterable of length {}'.format(length)
        raise ValueError(msg)

    return arr


cdef _get_gradient_stops(stops):
    cdef double[:,::1] arr = numpy.asarray(stops,
                                           dtype=numpy.float64,
                                           order='c')
    if arr.shape[1] != 5:
        msg = ('stops argument must be an iterable of (off, r, g, b, a) '
                'tuples.')
        raise ValueError(msg)

    return arr


@cython.internal
cdef class Paint:
    cdef _paint.Paint* _this

    def __dealloc__(self):
        del self._this


cdef class LinearGradientPaint(Paint):
    """ LinearGradientPaint(points, stops, spread, units)
          x1, y1: Gradient start point (x, y)
          x2, y2: Gradient end point (x, y)
          stops: An iterable of gradient stops (offset, r, g, b, a)
          spread: The GradientSpread type for this gradient
          units: The GradientUnits type for this gradient
    """
    cdef object _points
    cdef object _stops

    def __cinit__(self, double x1, double y1, double x2, double y2,
                  stops, GradientSpread spread, GradientUnits units):
        cdef double[::1] points_npy
        cdef double[:,::1] stops_npy = _get_gradient_stops(stops)
        points_npy = _get_gradient_points((x1, y1, x2, y2), 4)
        self._this = new _paint.Paint(
            _enums.k_PaintTypeLinearGradient,
            &points_npy[0], points_npy.shape[0],
            &stops_npy[0][0], stops_npy.shape[0], spread, units
        )

        # Hold on to the allocated arrays for safety
        self._points = points_npy
        self._stops = stops_npy


cdef class RadialGradientPaint(Paint):
    """ RadialGradientPaint(points, stops, spread, units)
          cx, cy: Gradient center point (x, y)
          r: Gradient radius
          fx, fy: Gradient focus point (x, y)
          stops: An iterable of gradient stops (offset, r, g, b, a)
          spread: The GradientSpread type for this gradient
          units: The GradientUnits type for this gradient
    """
    cdef object _points
    cdef object _stops

    def __cinit__(self, double cx, double cy, double r, double fx, double fy,
                  stops, GradientSpread spread, GradientUnits units):
        cdef double[::1] points_npy
        cdef double[:,::1] stops_npy = _get_gradient_stops(stops)
        points_npy = _get_gradient_points([cx, cy, r, fx, fy], 5)
        self._this = new _paint.Paint(
            _enums.k_PaintTypeRadialGradient,
            &points_npy[0], points_npy.shape[0],
            &stops_npy[0][0], stops_npy.shape[0], spread, units
        )

        # Hold on to the allocated arrays for safety
        self._points = points_npy
        self._stops = stops_npy


cdef class SolidPaint(Paint):
    """ SolidPaint(r, g, b, a)
    """
    def __cinit__(self, double r, double g, double b, double a=1.0):
        self._this = new _paint.Paint(r, g, b, a)

    def __repr__(self):
        name = type(self).__name__
        return "{}({}, {}, {}, {})".format(
            name, self._this.r(), self._this.g(), self._this.b(),
            self._this.a()
        )

    def __richcmp__(SolidPaint self, SolidPaint other, int op):
        if op == 2:  # ==
            return (self._this.a() == other._this.a() and
                    self._this.r() == other._this.r() and
                    self._this.g() == other._this.g() and
                    self._this.b() == other._this.b())
        else:
            msg = "That type of comparison is not implemented for SolidPaint"
            raise NotImplementedError(msg)

    property a:
        def __get__(self):
            return self._this.a()
        def __set__(self, double value):
            self._this.a(value)

    property r:
        def __get__(self):
            return self._this.r()
        def __set__(self, double value):
            self._this.r(value)

    property g:
        def __get__(self):
            return self._this.g()
        def __set__(self, double value):
            self._this.g(value)

    property b:
        def __get__(self):
            return self._this.b()
        def __set__(self, double value):
            self._this.b(value)
