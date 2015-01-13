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
from libc.stdint cimport (uint8_t, uint16_t, uint64_t)
cimport _pyagg

cdef class _ndarray_canvas_base_uint8:
    cdef _pyagg.ndarray_canvas_base[uint8_t]* _this
    cdef uint8_t[::1] default_color
    cdef uint64_t blend_channel_count
    cdef object py_image

    cdef int base_init(self, image, default_color, bool canvas_has_alpha) except -1:
        cdef uint64_t[:] image_shape = numpy.asarray(image.shape, dtype=numpy.uint64)
        cdef uint64_t image_ndim = <uint64_t> image.ndim
        cdef uint64_t required_channel_count
        self.default_color = numpy.asarray(default_color, dtype=numpy.uint8, order='c')
        required_channel_count = <uint64_t> self.default_color.shape[0]
        self.blend_channel_count = required_channel_count
        if not canvas_has_alpha:
            # Draw colors for images without alpha channels are specified with alpha channels and the drawing
            # operations are blended as if all image alpha channel pixels are at max value.  Therefore,
            # required channel count is one less than default color channel count if image does not have
            # an alpha channel.
            required_channel_count -= 1
        if image is None:
            raise ValueError('image argument must not be None.')
        if required_channel_count > 1 and (image_ndim != 3 or image_shape[2] != required_channel_count) or \
           required_channel_count == 1 and image_ndim != 2:
            if required_channel_count > 1:
                e = 'MxNx{}'.format(required_channel_count)
            else:
                e = 'MxN'
            raise ValueError('image argument must be {} channel (ie, must be {}).'.format(required_channel_count, e))
        # In order to ensure that our backing memory is not deallocated from underneath us, we retain a
        # reference to the memory view supplied to the constructor (image) as self.py_image, to be kept in
        # lock step with ndarray_canvas<>'s reference to that memory and destroyed when that reference
        # is lost.
        self.py_image = image
        return 0

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
            if type(c) in (float, int):
                c = (c,) # Cython's numpy.asarray requires an iterable
            c_npy = numpy.asarray(c, dtype=numpy.uint8, order='c')
            if c_npy.ndim > 1:
                raise ValueError('{} argument must be a flat iterable (except in the case of grayscale canvases, for which a scalar value is acceptable).'.format(n))
            if c_npy.shape[0] == self.blend_channel_count - 1:
                c_npy = numpy.hstack((c_npy, 255)).astype(numpy.uint8)
            elif c_npy.shape[0] != self.blend_channel_count:
                raise ValueError('If {} argument is not None, it must contain {} or {} elements.'.format(n, self.blend_channel_count-1, self.blend_channel_count))
        return c_npy

    def draw_line(self, x0, y0, x1, y1, w=1, c=None, aa=True):
        """draw_line(self, x0, y0, x1, y1, w=1, c=(255,...), aa=True):
          x0...y1: Start and endpoint coordinates
          w: Width
          c: Iterable of color components; components are 8-bit unsigned integers in the range [0,255]
          aa: If True, rendering is anti-aliased"""
        cdef uint8_t[::1] c_npy = self.get_color(c, 'line color')
        self._this.draw_line(x0, y0, x1, y1, w, &c_npy[0], aa)

    def draw_polygon(self, points,
                     line=True, line_w=1, line_c=None,
                     fill=False, fill_c=None,
                     aa=True):
        """draw_polygon(self, points, line=True, line_w=1, line_c=None, fill=False, fill_c=None, aa=True):
         points: Iterable of x,y pairs
         line: If True, line is drawn
         line_w: Outline width
         line_c: Outline color, an iterable of color components; components are 8-bit unsigned integers in the range [0,255]
         fill: If True, polygon is even/odd filled
         fill_c: Fill color
         aa: If True, rendering is anti-aliased"""
        cdef double[:,::1] points_npy = numpy.asarray(points, dtype=numpy.float64, order='c')
        cdef uint8_t[::1] line_c_npy
        cdef uint8_t[::1] fill_c_npy
        if points_npy.shape[1] != 2:
            raise ValueError('points argument must be an iterable of (x, y) pairs.')
        line_c_npy = self.get_color(line_c, 'line color')
        fill_c_npy = self.get_color(fill_c, 'fill color')
        self._this.draw_polygon(&points_npy[0][0], points_npy.shape[0],
                                line, line_w, &line_c_npy[0],
                                fill, &fill_c_npy[0],
                                aa)

    def draw_bezier3(self, x0, y0, x_ctrl, y_ctrl, x1, y1, w=1, c=None, aa=True):
        """draw_bezier3(self, x0, y0, x_ctrl, y_ctrl, x1, y1, w=1, c=None, aa=True):
         x0, y0: Start point
         x_ctrl, y_ctrl: Control point
         x1, y1: End point
         c: Iterable of color components; components are 8-bit unsigned integers in the range [0,255]
         aa: If True, rendering is anti-aliased

        Draws a quadratic Bezier curve."""
        cdef uint8_t[::1] c_npy = self.get_color(c, 'quadratic Bezier color')
        self._this.draw_bezier3(x0, y0, x_ctrl, y_ctrl, x1, y1, w, &c_npy[0], aa)

    def draw_bezier3_composite(self, points,
                               line=True, line_w=1, line_c=None,
                               fill=False, fill_c=None,
                               aa=True):
        """draw_bezier3_composite(self, points, line=True, line_w=1, line_c=None, fill=False, fill_c=None, aa=True):
         points: Iterable of x,y pairs defining a series of quadratic Bezier curves with the first pair being the start
          of the first curve, the next being the first curve's control point, and the following being the end point
          of the first curve and start point of the second (or simply the endpoint if there is no subsequent curve).
          For a series of N curves, points may look like the following:
              [[start0x, start0y],
               [control0x, control0y],
               [end0start1x, end0start1y],
               ...,
               [endMstartNx, endMstartNy],
               [controlNx, controlNy],
               [endNx, endNy]]
         line_c: Curve line color, an iterable of color components; components are 8-bit unsigned integers in the range [0,255]
         fill: If True, the region enclosed by the curves is even/odd filled
         fill_c: Fill color
         aa: If True, rendering is anti-aliased

        Draws a series of quadratic Bezier curves with optional even/odd filling."""
        cdef double[:,::1] points_npy = numpy.asarray(points, dtype=numpy.float64, order='c')
        cdef uint8_t[::1] line_c_npy
        cdef uint8_t[::1] fill_c_npy
        if points_npy.shape[1] != 2:
            raise ValueError('points argument must be an iterable of (x, y) pairs.')
        if points_npy.shape[0] < 3:
            raise ValueError('points argument must contain at least 3 (x, y) pairs.')
        if (points_npy.shape[0] - 1) % 2:
            raise ValueError('points argument must consist of a start point followed by at least one pair of control and start/end points.')
        line_c_npy = self.get_color(line_c, 'composite quadratic Bezier line color')
        fill_c_npy = self.get_color(fill_c, 'composite quadratic Bezier fill color')
        self._this.draw_bezier3_composite(&points_npy[0][0], points_npy.shape[0],
                                          line, line_w, &line_c_npy[0],
                                          fill, &fill_c_npy[0],
                                          aa)

    def draw_bezier4(self, x0, y0, x_ctrl0, y_ctrl0, x_ctrl1, y_ctrl1, x1, y1, w=1, c=None, aa=True):
        """draw_bezier3(self, x0, y0, x_ctrl0, y_ctrl0, x_ctrl1, y_ctrl1, x1, y1, w=1, c=None, aa=True):
         x0, y0: Start point
         x_ctrl0, y_ctrl0: control point A
         x_ctrl1, y_ctrl1: control point B
         x1, y1: End point
         c: Iterable of color components; components are 8-bit unsigned integers in the range [0,255]
         aa: If True, rendering is anti-aliased

        Draws a cubic Bezier curve."""
        cdef uint8_t[::1] c_npy = self.get_color(c, 'cubic Bezier color')
        self._this.draw_bezier4(x0, y0, x_ctrl0, y_ctrl0, x_ctrl1, y_ctrl1, x1, y1, w, &c_npy[0], aa)

    def draw_bezier4_composite(self, points,
                               line=True, line_w=1, line_c=None,
                               fill=False, fill_c=None,
                               aa=True):
        """draw_bezier4_composite(self, points, line=True, line_w=1, line_c=None, fill=False, fill_c=None, aa=True):
         points: Iterable of x,y pairs defining a series of cubic Bezier curves with the first pair being the start
          of the first curve, the next being the first curve's first control point, the next after being the first
          curve's second control point, and the following being the end point of the first curve and start point of
          the second (or simply the endpoint if there is no subsequent curve).
          For a series of N curves, points may look like the following:
              [[start0x, start0y],
               [control00x, control00y],
               [control10x, control10y],
               [end0start1x, end0start1y],
               ...,
               [endMstartNx, endMstartNy],
               [control0Nx, control0Ny],
               [control1Nx, control1Ny],
               [endNx, endNy]]
         line: If True, curve line is drawn (either of, or both of, line and fill may be True)
         line_w: Width of line in pixels
         line_c: Curve line color, an iterable of color components; components are 8-bit unsigned integers in the range [0,255]
         fill: If True, the region enclosed by the curves is even/odd filled
         fill_c: Fill color
         aa: If True, rendering is anti-aliased

        Draws a series of cubic Bezier curves with optional even/odd filling."""
        cdef double[:,::1] points_npy = numpy.asarray(points, dtype=numpy.float64, order='c')
        cdef uint8_t[::1] line_c_npy
        cdef uint8_t[::1] fill_c_npy
        if points_npy.shape[1] != 2:
            raise ValueError('points argument must be an iterable of (x, y) pairs.')
        if points_npy.shape[0] < 4:
            raise ValueError('points argument must contain at least 4 (x, y) pairs.')
        if (points_npy.shape[0] - 1) % 3:
            raise ValueError('points argument must consist of a start point followed by at least one set of a pair of control points and a start/end point.')
        line_c_npy = self.get_color(line_c, 'composite cubic Bezier line color')
        fill_c_npy = self.get_color(fill_c, 'composite cubic Bezier fill color')
        self._this.draw_bezier4_composite(&points_npy[0][0], points_npy.shape[0],
                                          line, line_w, &line_c_npy[0],
                                          fill, &fill_c_npy[0],
                                          aa)

    def draw_bspline(self, points,
                     line=True, line_w=1, line_c=None,
                     fill=False, fill_c=None,
                     aa=True):
        """draw_bspline(self, points, line=True, line_w=1, line_c=None, fill=False, fill_c=None, aa=True):
         points: Iterable of x,y pairs representing B-spline control points
         line: If True, curve line is drawn (either of, or both of, line and fill may be True)
         line_w: Width of line in pixels
         line_c: Curve line color, an iterable of color components; components are 8-bit unsigned integers in the range [0,255]
         fill: If True, the region enclosed by the B-spline is even/odd filled
         fill_c: Fill color
         aa: If True, rendering is anti-aliased"""
        cdef double[:,::1] points_npy = numpy.asarray(points, dtype=numpy.float64, order='c')
        cdef uint8_t[::1] line_c_npy
        cdef uint8_t[::1] fill_c_npy
        if points_npy.shape[1] != 2:
            raise ValueError('points argument must be an iterable of (x, y) pairs.')
        line_c_npy = self.get_color(line_c, 'bspline line color')
        fill_c_npy = self.get_color(fill_c, 'bspline fill color')
        self._this.draw_bspline(&points_npy[0][0], points_npy.shape[0],
                                line, line_w, &line_c_npy[0],
                                fill, &fill_c_npy[0],
                                aa)

cdef class ndarray_canvas_rgb24(_ndarray_canvas_base_uint8):
    """ndarray_canvas_rgb24 provides AGG (Anti-Grain Geometry) drawing routines that render to the numpy
    array passed as ndarray_canvas_rgb24's constructor argument.  Because this array is modified in place,
    it must be of type numpy.uint8, must be C-contiguous, and must be MxNx3 (3 channels: red, green, blue)."""
    def __cinit__(self, uint8_t[:,:,::1] image):
        self.base_init(image, (255,)*4, False)
        self._this = <_pyagg.ndarray_canvas_base[uint8_t]*> new _pyagg.ndarray_canvas[_pyagg.pixfmt_rgb24, uint8_t](&image[0][0][0], image.shape[1], image.shape[0], image.strides[0], 3)

cdef class ndarray_canvas_rgba32(_ndarray_canvas_base_uint8):
    """ndarray_canvas_rgba32 provides AGG (Anti-Grain Geometry) drawing routines that render to the numpy
    array passed as ndarray_canvas_rgba32's constructor argument.  Because this array is modified in place,
    it must be of type numpy.uint8, must be C-contiguous, and must be MxNx4 (4 channels: red, green, blue,
    and alpha)."""
    def __cinit__(self, uint8_t[:,:,::1] image):
        self.base_init(image, (255,)*4, True)
        self._this = <_pyagg.ndarray_canvas_base[uint8_t]*> new _pyagg.ndarray_canvas[_pyagg.pixfmt_rgba32, uint8_t](&image[0][0][0], image.shape[1], image.shape[0], image.strides[0], 4)

cdef class ndarray_canvas_g8(_ndarray_canvas_base_uint8):
    """ndarray_canvas_g16 provides AGG (Anti-Grain Geometry) drawing routines that render to the numpy
    array passed as ndarray_canvas_g16's constructor argument.  Because this array is modified in place,
    it must be of type numpy.uint8, must be C-contiguous, and must be MxN (1 channel: intensity)."""
    def __cinit__(self, uint8_t[:,::1] image):
        self.base_init(image, (255,)*2, False)
        self._this = <_pyagg.ndarray_canvas_base[uint8_t]*> new _pyagg.ndarray_canvas[_pyagg.pixfmt_gray8_no_alpha, uint8_t](&image[0][0], image.shape[1], image.shape[0], image.strides[0], 1)

cdef class ndarray_canvas_ga16(_ndarray_canvas_base_uint8):
    """ndarray_canvas_ga16 provides AGG (Anti-Grain Geometry) drawing routines that render to the numpy
    array passed as ndarray_canvas_ga16's constructor argument.  Because this array is modified in place,
    it must be of type numpy.uint8, must be C-contiguous, and must be MxNx2 (2 channels: intensity and
    alpha)."""
    def __cinit__(self, uint8_t[:,:,::1] image):
        self.base_init(image, (255,)*2, True)
        self._this = <_pyagg.ndarray_canvas_base[uint8_t]*> new _pyagg.ndarray_canvas[_pyagg.pixfmt_gray8, uint8_t](&image[0][0][0], image.shape[1], image.shape[0], image.strides[0], 2)

# TODO: make the 16 bit integer per channel stuff commented out below work (the problem may just be that AGG wants
# buffers to always be unsigned chars, which we could work around with a typecast).
# 
# Ambitious TODO: Add floating point pixel formats, renderers, and cython interface
#
#cdef class _ndarray_canvas_base_uint16:
#    cdef uint16_t[:,:,::1] py_image
#    cdef _pyagg.ndarray_canvas_base[uint16_t]* _this
#    cdef uint16_t[::1] default_color
#
#    cdef void base_init(self, uint16_t[:,:,::1] image, channel_count, default_color):
#        if image is None:
#            raise ValueError('image argument must not be None.')
#        if image.shape[2] != channel_count:
#            raise ValueError('image argument must be {0} channel (ie, must be MxNx{0}).'.format(channel_count))
#        # In order to ensure that our backing memory is not deallocated from underneath us, we retain a
#        # reference to the memory view supplied to the constructor (image) as self.py_image, to be kept in
#        # lock step with ndarray_canvas<>'s reference to that memory and destroyed when that reference
#        # is lost.
#        self.py_image = image
#        self.default_color = numpy.asarray(default_color, dtype=numpy.uint16, order='c')
#
#    def __dealloc__(self):
#        del self._this
#
#    @property
#    def channel_count(self):
#        return self._this.channel_count()
#
#    @property
#    def width(self):
#        return self._this.width()
#
#    @property
#    def height(self):
#        return self._this.height()
#
#    @property
#    def image(self):
#        # User is not likely to be concerned with the details of the cython memory view around array or buffer
#        # supplied to the constructor or attach, but instead expects the original array or buffer (which is why
#        # self.py_image.base is returned rather than self.py_image).
#        return self.py_image.base
#
#    cdef uint16_t[::1] get_color(self, c, n):
#        cdef uint16_t[::1] c_npy
#        if c is None:
#            c_npy = self.default_color
#        else:
#            c_npy = numpy.asarray(c, dtype=numpy.uint16, order='c')
#            if c_npy.ndim > 1:
#                raise ValueError('{} argument must be a flat iterable.'.format(n))
#            if c_npy.shape[0] != self._this.channel_count():
#                raise ValueError('{} argument must contain {} element(s).'.format(self._this.channel_count()))
#        return c_npy
#
#    def draw_line(self, x0, y0, x1, y1, w=1, c=None, aa=True):
#        """draw_line(self, x0, y0, x1, y1, w=1, c=(255,...), aa=True):
#        x0...y1: start and endpoint coordinates; floating point values
#        w: width; floating point value
#        c: iterable of color components; components are 8-bit unsigned integers in the range [0,255]
#        aa: if True, rendering is anti-aliased"""
#        cdef uint16_t[::1] c_npy = self.get_color(c, 'line color')
#        self._this.draw_line(x0, y0, x1, y1, w, &c_npy[0], aa)
#
#    def draw_polygon(self, points,
#                     outline=True, outline_w=1,
#                     outline_c=None,
#                     fill=False,
#                     fill_c=None,
#                     aa=True):
#        """draw_polygon(self, points, outline=True, outline_w=1, outline_c=None, fill=False, fill_c=None, aa=True):
#        points: iterable of x,y floating point coordinate pairs
#        outline: if True, outline is drawn
#        outline_w: outline width; floating point value
#        outline_c: outline color, an iterable of color components; components are 8-bit unsigned integers in the range [0,255]
#        fill: if True, polygon is filled
#        fill_c: polygon fill color
#        aa: if True, rendering is anti-aliased"""
#        cdef uint16_t[::1] outline_c_npy = self.get_color(outline_c, 'outline color')
#        cdef uint16_t[::1] fill_c_npy = self.get_color(fill_c, 'fill color')
#        cdef double[:,::1] points_npy = numpy.asarray(points, dtype=numpy.float64, order='c')
#        if points_npy.shape[1] != 2:
#            raise ValueError('Points argument must be an iterable of (x, y) pairs.')
#        self._this.draw_polygon(&points_npy[0][0], points_npy.shape[0],
#                                outline, outline_w, &outline_c_npy[0],
#                                fill, &fill_c_npy[0],
#                                aa)

#cdef class ndarray_canvas_rgb48(_ndarray_canvas_base_uint16):
#    """ndarray_canvas_rgb48 provides AGG (Anti-Grain Geometry) drawing routines that render to the numpy
#    array passed as ndarray_canvas_rgb48's constructor argument.  Because this array is modified in place,
#    it must be of type numpy.uint16, must be C-contiguous, and must be MxNx3 (3 channels: red, green, blue)."""
#    def __cinit__(self, uint16_t[:,:,::1] image):
#        self.base_init(image, 3, (65535,)*3)
#        self._this = <_pyagg.ndarray_canvas_base[uint16_t]*> new _pyagg.ndarray_canvas[_pyagg.pixfmt_rgb48, uint16_t](&image[0][0][0], image.shape[1], image.shape[0], image.strides[0], 3)

#       cdef class ndarray_canvas_rgba64(_ndarray_canvas_base_uint16):
#           """ndarray_canvas_rgba64 provides AGG (Anti-Grain Geometry) drawing routines that render to the numpy
#           array passed as ndarray_canvas_rgba32's constructor argument.  Because this array is modified in place,
#           it must be of type numpy.uint16, must be C-contiguous, and must be MxNx4 (4 channels: red, green, blue,
#           and alpha)."""
#           def __cinit__(self, uint16_t[:,:,::1] image):
#               '''hello worlddd'''
#               self.base_init(image, 4, (65535,)*4)
#               self._this = <_pyagg.ndarray_canvas_base[uint16_t]*> new _pyagg.ndarray_canvas[_pyagg.pixfmt_rgba64, uint16_t](&image[0][0][0], image.shape[1], image.shape[0], image.strides[0], 4)
#
#       cdef class ndarray_canvas_ga32(_ndarray_canvas_base_uint16):
#           """ndarray_canvas_ga16 provides AGG (Anti-Grain Geometry) drawing routines that render to the numpy
#           array passed as ndarray_canvas_ga16's constructor argument.  Because this array is modified in place,
#           it must be of type numpy.uint16, must be C-contiguous, and must be MxNx2 (2 channels: intensity and
#           alpha)."""
#           def __cinit__(self, uint16_t[:,:,::1] image):
#               self.base_init(image, 2, (65535,)*2)
#               self._this = <_pyagg.ndarray_canvas_base[uint16_t]*> new _pyagg.ndarray_canvas[_pyagg.pixfmt_gray16, uint16_t](&image[0][0][0], image.shape[1], image.shape[0], image.strides[0], 2)
