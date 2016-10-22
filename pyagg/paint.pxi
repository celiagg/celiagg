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

cpdef enum GradientSpread:
    SpreadPad = _enums.k_GradientSpreadPad
    SpreadReflect = _enums.k_GradientSpreadReflect
    SpreadRepeat = _enums.k_GradientSpreadRepeat

cpdef enum GradientUnits:
    UserSpace = _enums.k_GradientUnitsUserSpace
    ObjectBoundingBox = _enums.k_GradientUnitsObjectBoundingBox

cpdef enum PatternStyle:
    StyleRepeat = _enums.k_PatternStyleRepeat
    StyleReflect = _enums.k_PatternStyleReflect


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

    property transform:
        def __get__(self):
            cdef Transform trans = Transform()
            trans._assign_obj(self._this.transform())
            return trans
        def __set__(self, Transform trans):
            self._this.transform(dereference(trans._this))


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
        cdef double[::1] _points = _get_gradient_points((x1, y1, x2, y2), 4)
        cdef double[:,::1] _stops = _get_gradient_stops(stops)
        self._this = new _paint.Paint(_enums.k_PaintTypeLinearGradient,
                                      &_points[0], _points.shape[0],
                                      &_stops[0][0], _stops.shape[0],
                                      spread, units)

        # Hold on to the allocated arrays for safety
        self._points = _points
        self._stops = _stops

    def copy(self):
        points = self._points  # Not a deep copy!
        stops = self._stops  # Not a deep copy!
        cpy = LinearGradientPaint(
            points[0], points[1], points[2], points[3],
            stops, self._this.spread(), self._this.units()
        )
        cpy.transform = self.transform
        return cpy


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
        cdef double[::1] _points = _get_gradient_points([cx, cy, r, fx, fy], 5)
        cdef double[:,::1] _stops = _get_gradient_stops(stops)
        self._this = new _paint.Paint(_enums.k_PaintTypeRadialGradient,
                                      &_points[0], _points.shape[0],
                                      &_stops[0][0], _stops.shape[0],
                                      spread, units)

        # Hold on to the allocated arrays for safety
        self._points = _points
        self._stops = _stops

    def copy(self):
        points = self._points  # Not a deep copy!
        stops = self._stops  # Not a deep copy!
        cpy = RadialGradientPaint(
            points[0], points[1], points[2], points[3], points[4],
            stops, self._this.spread(), self._this.units()
        )
        cpy.transform = self.transform
        return cpy


cdef class PatternPaint(Paint):
    """ PatternPaint(style, image)
          style: A PatternStyle
          image: An Image object
    """
    cdef PatternStyle style
    cdef object img_obj

    def __cinit__(self, PatternStyle style, Image image):
        self._this = new _paint.Paint(style, image._this)

        # Hold a reference to the pattern image
        self.img_obj = image
        self.style = style

    def _with_format(self, PixelFormat fmt):
        if self.img_obj.format == fmt:
            return self

        fmt_image = convert_image(self.img_obj, fmt)
        fmt_pattern = PatternPaint(self.style, fmt_image)
        fmt_pattern.transform = self.transform
        return fmt_pattern

    def copy(self):
        img = self.img_obj.copy()
        cpy = PatternPaint(self.style, img)
        cpy.transform = self.transform
        return cpy


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

    def copy(self):
        cpy = SolidPaint(self._this.r(), self._this.g(), self._this.b(),
                         self._this.a())
        cpy.transform = self.transform
        return cpy

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
