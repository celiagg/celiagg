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

cdef class ndarray_canvas_rgb8:
    cdef _pyagg.ndarray_canvas[_pyagg.pixfmt_rgb24]* _this
    cdef uint8_t[:,:,::1] py_image

    def __cinit__(self, uint8_t[:,:,::1] image):
        if image is None:
            raise ValueError('image argument must not be None.')
        if image.shape[2] != 3:
            raise ValueError('image argument must have three channels (ie, must be MxNx3).')
        # In order to ensure that our backing memory is not deallocated from underneath us, we retain a
        # reference to the memory view supplied to the constructor (image) as self.py_image, to be kept in
        # lock step with SimpleAgg<>'s reference to that memory and destroyed when that reference
        # is lost.
        self.py_image = image
        self._this = new _pyagg.ndarray_canvas[_pyagg.pixfmt_rgb24](&image[0][0][0], image.shape[1], image.shape[0], image.strides[0])

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

    def draw_line(self, x0, y0, x1, y1, w=1, c=(255,255,255), aa=True):
        cdef uint8_t[::1] c_npy = numpy.asarray(c, dtype=numpy.uint8, order='c')
        self._this.draw_line(x0, y0, x1, y1, w, &c_npy[0], aa)

    def draw_polygon(self, points,
                     outline=True, outline_w=1,
                     outline_c=(255,255,255),
                     fill=False,
                     fill_c=(255,255,255),
                     aa=True):
        cdef uint8_t[::1] outline_c_npy = numpy.asarray(outline_c, dtype=numpy.uint8, order='c')
        cdef uint8_t[::1] fill_c_npy = numpy.asarray(fill_c, dtype=numpy.uint8, order='c')
        cdef double[:,::1] points_npy = numpy.asarray(points, dtype=numpy.float64, order='c')
        if points_npy.shape[1] != 2:
            raise ValueError('Points argument must be an iterable of (x, y) pairs.')
        self._this.draw_polygon(&points_npy[0][0], points_npy.shape[0],
                                outline, outline_w, &outline_c_npy[0],
                                fill, &fill_c_npy[0],
                                aa)

cdef class ndarray_canvas_rgba8:
    cdef _pyagg.ndarray_canvas[_pyagg.pixfmt_rgba32]* _this
    cdef uint8_t[:,:,::1] py_image

    def __cinit__(self, uint8_t[:,:,::1] image):
        if image is None:
            raise ValueError('image argument must not be None.')
        if image.shape[2] != 4:
            raise ValueError('image argument must have four channels (ie, must be MxNx4).')
        # In order to ensure that our backing memory is not deallocated from underneath us, we retain a
        # reference to the memory view supplied to the constructor (image) as self.py_image, to be kept in
        # lock step with SimpleAgg<>'s reference to that memory and destroyed when that reference
        # is lost.
        self.py_image = image
        self._this = new _pyagg.ndarray_canvas[_pyagg.pixfmt_rgba32](&image[0][0][0], image.shape[1], image.shape[0], image.strides[0])

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

    def draw_line(self, x0, y0, x1, y1, w=1, c=(255, 255, 255, 255), aa=True):
        cdef uint8_t[::1] c_npy = numpy.asarray(c, dtype=numpy.uint8, order='c')
        self._this.draw_line(x0, y0, x1, y1, w, &c_npy[0], aa)

    def draw_polygon(self, points,
                     outline=True, outline_w=1,
                     outline_c=(255, 255, 255, 255),
                     fill=False,
                     fill_c=(255, 255, 255, 255),
                     aa=True):
        cdef uint8_t[::1] outline_c_npy = numpy.asarray(outline_c, dtype=numpy.uint8, order='c')
        cdef uint8_t[::1] fill_c_npy = numpy.asarray(fill_c, dtype=numpy.uint8, order='c')
        cdef double[:,::1] points_npy = numpy.asarray(points, dtype=numpy.float64, order='c')
        if points_npy.shape[1] != 2:
            raise ValueError('Points argument must be an iterable of (x, y) pairs.')
        self._this.draw_polygon(&points_npy[0][0], points_npy.shape[0],
                                outline, outline_w, &outline_c_npy[0],
                                fill, &fill_c_npy[0],
                                aa)

cdef class ndarray_canvas_grayscale_alpha8:
    cdef _pyagg.ndarray_canvas[_pyagg.pixfmt_gray8]* _this
    cdef uint8_t[:,:,::1] py_image

    def __cinit__(self, uint8_t[:,:,::1] image):
        if image is None:
            raise ValueError('image argument must not be None.')
        if image.shape[2] != 2:
            raise ValueError('image argument must have two channels (ie, must be MxNx2).')
        # In order to ensure that our backing memory is not deallocated from underneath us, we retain a
        # reference to the memory view supplied to the constructor (image) as self.py_image, to be kept in
        # lock step with SimpleAgg<>'s reference to that memory and destroyed when that reference
        # is lost.
        self.py_image = image
        self._this = new _pyagg.ndarray_canvas[_pyagg.pixfmt_gray8](&image[0][0][0], image.shape[1], image.shape[0], image.strides[0])

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

    def draw_line(self, x0, y0, x1, y1, w=1, c=(255, 255), aa=True):
        cdef uint8_t[::1] c_npy = numpy.asarray(c, dtype=numpy.uint8, order='c')
        self._this.draw_line(x0, y0, x1, y1, w, &c_npy[0], aa)

    def draw_polygon(self, points,
                     outline=True, outline_w=1,
                     outline_c=(255, 255),
                     fill=False,
                     fill_c=(255, 255),
                     aa=True):
        cdef uint8_t[::1] outline_c_npy = numpy.asarray(outline_c, dtype=numpy.uint8, order='c')
        cdef uint8_t[::1] fill_c_npy = numpy.asarray(fill_c, dtype=numpy.uint8, order='c')
        cdef double[:,::1] points_npy = numpy.asarray(points, dtype=numpy.float64, order='c')
        if points_npy.shape[1] != 2:
            raise ValueError('Points argument must be an iterable of (x, y) pairs.')
        self._this.draw_polygon(&points_npy[0][0], points_npy.shape[0],
                                outline, outline_w, &outline_c_npy[0],
                                fill, &fill_c_npy[0],
                                aa)
