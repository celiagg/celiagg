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

cdef class _ndarray_canvas_base_uint8:
    cdef uint8_t[:,:,::1] py_image
    cdef _pyagg.ndarray_canvas_base[uint8_t]* _this
    cdef uint8_t[::1] default_color

    cdef void base_init(self, uint8_t[:,:,::1] image, channel_count, default_color):
        if image is None:
            raise ValueError('image argument must not be None.')
        if image.shape[2] != channel_count:
            raise ValueError('image argument must be {0} channel (ie, must be MxNx{0}).'.format(channel_count))
        # In order to ensure that our backing memory is not deallocated from underneath us, we retain a
        # reference to the memory view supplied to the constructor (image) as self.py_image, to be kept in
        # lock step with ndarray_canvas<>'s reference to that memory and destroyed when that reference
        # is lost.
        self.py_image = image
        self.default_color = numpy.asarray(default_color, dtype=numpy.uint8, order='c')

    def __dealloc__(self):
        del self._this

    @property
    def channel_count(self):
        return self._this.channel_count()

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

    cdef uint8_t[::1] get_color(self, c, n):
        cdef uint8_t[::1] c_npy
        if c is None:
            c_npy = self.default_color
        else:
            c_npy = numpy.asarray(c, dtype=numpy.uint8, order='c')
            if c_npy.ndim > 1:
                raise ValueError('{} argument must be a flat iterable.'.format(n))
            if c_npy.shape[0] != self._this.channel_count():
                raise ValueError('{} argument must contain {} element(s).'.format(self._this.channel_count()))
        return c_npy

    def draw_line(self, x0, y0, x1, y1, w=1, c=None, aa=True):
        cdef uint8_t[::1] c_npy = self.get_color(c, 'line color')
        self._this.draw_line(x0, y0, x1, y1, w, &c_npy[0], aa)

    def draw_polygon(self, points,
                     outline=True, outline_w=1,
                     outline_c=(255,255,255),
                     fill=False,
                     fill_c=(255,255,255),
                     aa=True):
        cdef uint8_t[::1] outline_c_npy = self.get_color(outline_c, 'outline color')
        cdef uint8_t[::1] fill_c_npy = self.get_color(fill_c, 'fill color')
        cdef double[:,::1] points_npy = numpy.asarray(points, dtype=numpy.float64, order='c')
        if points_npy.shape[1] != 2:
            raise ValueError('Points argument must be an iterable of (x, y) pairs.')
        self._this.draw_polygon(&points_npy[0][0], points_npy.shape[0],
                                outline, outline_w, &outline_c_npy[0],
                                fill, &fill_c_npy[0],
                                aa)

cdef class ndarray_canvas_rgb24(_ndarray_canvas_base_uint8):
    def __cinit__(self, uint8_t[:,:,::1] image):
        self.base_init(image, 3, (255,255,255))
        self._this = <_pyagg.ndarray_canvas_base[uint8_t]*> new _pyagg.ndarray_canvas[_pyagg.pixfmt_rgb24](&image[0][0][0], image.shape[1], image.shape[0], image.strides[0], 3)

cdef class ndarray_canvas_rgba32(_ndarray_canvas_base_uint8):
    def __cinit__(self, uint8_t[:,:,::1] image):
        self.base_init(image, 4, (255,255,255,255))
        self._this = <_pyagg.ndarray_canvas_base[uint8_t]*> new _pyagg.ndarray_canvas[_pyagg.pixfmt_rgba32](&image[0][0][0], image.shape[1], image.shape[0], image.strides[0], 4)

cdef class ndarray_canvas_ga16(_ndarray_canvas_base_uint8):
    def __cinit__(self, uint8_t[:,:,::1] image):
        self.base_init(image, 2, (255,255))
        self._this = <_pyagg.ndarray_canvas_base[uint8_t]*> new _pyagg.ndarray_canvas[_pyagg.pixfmt_gray8](&image[0][0][0], image.shape[1], image.shape[0], image.strides[0], 2)
