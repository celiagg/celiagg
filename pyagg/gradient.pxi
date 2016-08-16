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
# Authors: John Wiggins

cdef class Gradient:
    pass


cdef class LinearGradient(Gradient):
    cdef _gradient.LinearGradientByte* _this

    def __cinit__(self, double x1, double y1, double x2, double y2,
                  Color c1, Color c2, double profile):
        self._this = new _gradient.LinearGradientByte(x1, y1, x2, y2,
                                                      c1._this[0], c2._this[0],
                                                      profile)

    def __dealloc__(self):
        del self._this


cdef class RadialGradient(Gradient):
    cdef _gradient.RadialGradientByte* _this

    def __cinit__(self, double x, double y, double r, Color c1, Color c2,
                  double profile):
        self._this = new _gradient.RadialGradientByte(x, y, r,
                                                      c1._this[0], c2._this[0],
                                                      profile)

    def __dealloc__(self):
        del self._this
