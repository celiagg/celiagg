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

from libcpp cimport bool
import cython
cimport numpy
import numpy
from libc.stdint cimport uint8_t
cimport _pyagg

cdef class ndarray_canvas:
    cdef _pyagg.ndarray_canvas* _this
    cdef uint8_t[:,:,::1] py_image

    def __cinit__(self, uint8_t[:,:,::1] image):
        if image is None:
            raise ValueError('image argument must not be None.')
        else:
            # In order to ensure that our backing memory is not deallocated from underneath us, we retain a
            # reference to the memory view supplied to the constructor (image) as self.py_image, to be kept in
            # lock step with SimpleAgg<>'s reference to that memory and destroyed when that reference
            # is lost.
            self.py_image = image
            self._this = new _pyagg.ndarray_canvas(&image[0][0][0], image.shape[1], image.shape[0], image.strides[0])

    def __dealloc__(self):
        del self._this

    @property
    def width(self):
        return self._this.width()

    @property
    def height(self):
        return self._this.height()

    @property
    def image(self):
        # User is not likely to be concerned with the details of the cython memory view around array or buffer
        # supplied to the constructor or attach, but instead expects the original array or buffer (which is why
        # self.py_image.base is returned rather than self.py_image).
        return self.py_image.base

    def draw_line(self, x0, y0, x1, y1, w=1, r=255, g=255, b=255, a=255, aa=True):
        self._this.draw_line(x0, y0, x1, y1, w, r, g, b, a, aa)

    def draw_polygon(self, points,
                     outline=True, outline_w=1,
                     outline_r=255, outline_g=255, outline_b=255, outline_a=255,
                     fill=False,
                     fill_r=0, fill_g=0, fill_b=0, fill_a=255,
                     aa=True):
        cdef double[:,::1] points_npy = numpy.asarray(points, dtype=numpy.float64, order='c')
        if points_npy.shape[1] != 2:
            raise ValueError('Points argument must be an iterable of (x, y) pairs.')
        self._this.draw_polygon(&points_npy[0][0], points_npy.shape[0],
                                outline, outline_w,
                                outline_r, outline_g, outline_b, outline_a,
                                fill,
                                fill_r, fill_g, fill_b, fill_a,
                                aa)
