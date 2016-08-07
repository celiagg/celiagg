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
# Authors: Erik Hvatum <ice.rikh@gmail.com>

ctypedef _pyagg.ndarray_canvas_base[uint8_t] canvas_base_t
ctypedef _pyagg.ndarray_canvas[_pyagg.pixfmt_rgba32, uint8_t] canvas_rgba32_t
ctypedef _pyagg.ndarray_canvas[_pyagg.pixfmt_rgb24, uint8_t] canvas_rgb24_t
ctypedef _pyagg.ndarray_canvas[_pyagg.pixfmt_gray8, uint8_t] canvas_ga16_t
ctypedef _pyagg.ndarray_canvas[_pyagg.pixfmt_gray8_no_alpha, uint8_t] canvas_g8_t

@cython.internal
cdef class ndarray_canvas_base_uint8:
    cdef canvas_base_t* _this
    cdef uint8_t[::1] default_color
    cdef uint64_t blend_channel_count
    cdef object py_image

    cdef int base_init(self, image, default_color, bool has_alpha) except -1:
        cdef uint64_t[:] image_shape = numpy.asarray(image.shape,
                                                     dtype=numpy.uint64,
                                                     order='c')
        cdef uint64_t image_ndim = <uint64_t> image.ndim
        cdef uint64_t required_channel_count
        self.default_color = numpy.asarray(default_color, dtype=numpy.uint8,
                                           order='c')
        required_channel_count = <uint64_t> self.default_color.shape[0]
        self.blend_channel_count = required_channel_count
        if not has_alpha:
            # Draw colors for images without alpha channels are specified with
            # alpha channels and the drawing operations are blended as if all
            # image alpha channel pixels are at max value.  Therefore, required
            # channel count is one less than default color channel count if
            # image does not have an alpha channel.
            required_channel_count -= 1
        if image is None:
            raise ValueError('image argument must not be None.')
        if (required_channel_count > 1 and (image_ndim != 3
                or image_shape[2] != required_channel_count) or
                required_channel_count == 1 and image_ndim != 2):
            if required_channel_count > 1:
                e = 'MxNx{}'.format(required_channel_count)
            else:
                e = 'MxN'
            msg_tmp = 'image argument must be {} channel (ie, must be {}).'
            raise ValueError(msg_tmp.format(required_channel_count, e))
        # In order to ensure that our backing memory is not deallocated from
        # underneath us, we retain a reference to the memory view supplied to
        # the constructor (image) as self.py_image, to be kept in lock step
        # with ndarray_canvas<>'s reference to that memory and destroyed when
        # that reference is lost.
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
        # User is not likely to be concerned with the details of the cython
        # memory view around array or buffer supplied to the constructor or
        # attach, but instead expects the original array or buffer (which is
        # why self.py_image.base is returned rather than self.py_image).
        return self.py_image.base

    def draw_line(self, x0, y0, x1, y1, GraphicsState state):
        """draw_line(self, x0, y0, x1, y1, state):
          x0...y1: Start and endpoint coordinates
          state: A GraphicsState object
                 line width, line color, anti-aliased
        """
        self._this.draw_line(x0, y0, x1, y1, state._this[0])

    def draw_polygon(self, points, GraphicsState state):
        """draw_polygon(self, points, state):
          points: Iterable of (x, y) pairs
          state: A GraphicsState object
                 line color, line width, fill color, anti-aliased
        """
        cdef double[:,::1] points_npy = numpy.asarray(points,
                                                      dtype=numpy.float64,
                                                      order='c')
        if points_npy.shape[1] != 2:
            msg = 'Points argument must be an iterable of (x, y) pairs.'
            raise ValueError(msg)

        self._this.draw_polygon(&points_npy[0][0], points_npy.shape[0],
                                state._this[0])

    def draw_ellipse(self, cx, cy, rx, ry, GraphicsState state):
        """draw_ellipse(self, cx, cy, rx, ry, state):
          cx, cy: Ellipse center
          rx, ry: Radius of ellipse along x and y axes
          state: A GraphicsState object
                 line color, line width, fill color, anti-aliased
        """
        self._this.draw_ellipse(cx, cy, rx, ry, state._this[0])

    def draw_bezier3(self, x0, y0, x_ctrl, y_ctrl, x1, y1, GraphicsState state):
        """draw_bezier3(self, x0, y0, x_ctrl, y_ctrl, x1, y1, state):
        Draws a quadratic Bezier curve.
          x0, y0: Start point
          x_ctrl, y_ctrl: Control point
          x1, y1: End point
          state: A GraphicsState object
                 line color, line width, anti-aliased
        """
        self._this.draw_bezier3(x0, y0, x_ctrl, y_ctrl, x1, y1, state._this[0])

    def draw_bezier3_composite(self, points, GraphicsState state):
        """draw_bezier3_composite(self, points, state):
        Draws a series of quadratic Bezier curves with optional even/odd filling.
          points: Iterable of (x, y) pairs defining a series of quadratic Bezier
                  curves with the first pair being the start of the first
                  curve, the next being the first curve's control point, and
                  the following being the end point of the first curve and
                  start point of the second (or simply the endpoint if there
                  is no subsequent curve).
                  For a series of N curves, points may look like the following:
                        [[start0x, start0y],
                        [control0x, control0y],
                        [end0start1x, end0start1y],
                        ...,
                        [endMstartNx, endMstartNy],
                        [controlNx, controlNy],
                        [endNx, endNy]]
          state: A GraphicsState object
                 line color, line width, fill color, anti-aliased
        """
        cdef double[:,::1] points_npy = numpy.asarray(points,
                                                      dtype=numpy.float64,
                                                      order='c')

        if points_npy.shape[1] != 2:
            raise ValueError('points argument must be an iterable of (x, y) pairs.')
        if points_npy.shape[0] < 3:
            raise ValueError('points argument must contain at least 3 (x, y) pairs.')
        if (points_npy.shape[0] - 1) % 2:
            msg = ('Points argument must consist of a start point followed by '
                   'at least one pair of control and start/end points.')
            raise ValueError(msg)

        self._this.draw_bezier3_composite(&points_npy[0][0], points_npy.shape[0],
                                          state._this[0])

    def draw_bezier4(self, x0, y0, x_ctrl0, y_ctrl0, x_ctrl1, y_ctrl1, x1, y1,
                     GraphicsState state):
        """draw_bezier3(self, x0, y0, x_ctrl0, y_ctrl0, x_ctrl1, y_ctrl1, x1,
                        y1, state):
        Draws a cubic Bezier curve.
          x0, y0: Start point
          x_ctrl0, y_ctrl0: control point A
          x_ctrl1, y_ctrl1: control point B
          x1, y1: End point
          state: A GraphicsState object
                 line color, line width, anti-aliased
        """
        self._this.draw_bezier4(x0, y0, x_ctrl0, y_ctrl0, x_ctrl1, y_ctrl1, x1,
                                y1, state._this[0])

    def draw_bezier4_composite(self, points, GraphicsState state):
        """draw_bezier4_composite(self, points, state):
        Draws a series of cubic Bezier curves with optional even/odd filling.
          points: Iterable of (x, y) pairs defining a series of cubic Bezier
                  curves with the first pair being the start of the first
                  curve, the next being the first curve's first control point,
                  the next after being the first curve's second control point,
                  and the following being the end point of the first curve and
                  start point of the second (or simply the endpoint if there is
                  no subsequent curve).
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
          state: A GraphicsState object
                 line color, line width, fill color, anti-aliased
        """
        cdef double[:,::1] points_npy = numpy.asarray(points,
                                                      dtype=numpy.float64,
                                                      order='c')

        if points_npy.shape[1] != 2:
            raise ValueError('points argument must be an iterable of (x, y) pairs.')
        if points_npy.shape[0] < 4:
            raise ValueError('points argument must contain at least 4 (x, y) pairs.')
        if (points_npy.shape[0] - 1) % 3:
            msg = ('points argument must consist of a start point followed by '
                   'at least one set of a pair of control points and a '
                   'start/end point.')
            raise ValueError(msg)

        self._this.draw_bezier4_composite(&points_npy[0][0], points_npy.shape[0],
                                          state._this[0])

    def draw_bspline(self, points, GraphicsState state):
        """draw_bspline(self, points, state):
          points: Iterable of (x, y) pairs representing B-spline control points
          state: A GraphicsState object
                 line color, line width, fill color, anti-aliased
        """
        cdef double[:,::1] points_npy = numpy.asarray(points,
                                                      dtype=numpy.float64,
                                                      order='c')

        if points_npy.shape[1] != 2:
            msg = 'Points argument must be an iterable of (x, y) pairs.'
            raise ValueError(msg)

        self._this.draw_bspline(&points_npy[0][0], points_npy.shape[0],
                                state._this[0])


cdef class ndarray_canvas_rgba32(ndarray_canvas_base_uint8):
    """ndarray_canvas_rgba32 provides AGG (Anti-Grain Geometry) drawing
    routines that render to the numpy array passed as ndarray_canvas_rgba32's
    constructor argument.  Because this array is modified in place, it must be
    of type numpy.uint8, must be C-contiguous, and must
    be MxNx4 (4 channels: red, green, blue, and alpha).
    """
    def __cinit__(self, uint8_t[:,:,::1] image):
        self.base_init(image, (255,)*4, True)
        self._this = <canvas_base_t*> new canvas_rgba32_t(&image[0][0][0],
                                                          image.shape[1],
                                                          image.shape[0],
                                                          image.strides[0], 4)


cdef class ndarray_canvas_rgb24(ndarray_canvas_base_uint8):
    """ndarray_canvas_rgb24 provides AGG (Anti-Grain Geometry) drawing routines
    that render to the numpy array passed as ndarray_canvas_rgb24's constructor
    argument.  Because this array is modified in place, it must be of type
    numpy.uint8, must be C-contiguous, and
    must be MxNx3 (3 channels: red, green, blue).
    """
    def __cinit__(self, uint8_t[:,:,::1] image):
        self.base_init(image, (255,)*4, False)
        self._this = <canvas_base_t*> new canvas_rgb24_t(&image[0][0][0],
                                                         image.shape[1],
                                                         image.shape[0],
                                                         image.strides[0], 3)


cdef class ndarray_canvas_ga16(ndarray_canvas_base_uint8):
    """ndarray_canvas_ga16 provides AGG (Anti-Grain Geometry) drawing routines
    that render to the numpy array passed as ndarray_canvas_ga16's constructor
    argument.  Because this array is modified in place, it must be of type
    numpy.uint8, must be C-contiguous, and must
    be MxNx2 (2 channels: intensity and alpha).
    """
    def __cinit__(self, uint8_t[:,:,::1] image):
        self.base_init(image, (255,)*2, True)
        self._this = <canvas_base_t*> new canvas_ga16_t(&image[0][0][0],
                                                        image.shape[1],
                                                        image.shape[0],
                                                        image.strides[0], 2)


cdef class ndarray_canvas_g8(ndarray_canvas_base_uint8):
    """ndarray_canvas_g8 provides AGG (Anti-Grain Geometry) drawing routines
    that render to the numpy array passed as ndarray_canvas_g8's constructor
    argument.  Because this array is modified in place, it must be of type
    numpy.uint8, must be C-contiguous, and must be MxN (1 channel: intensity).
    """
    def __cinit__(self, uint8_t[:,::1] image):
        self.base_init(image, (255,)*2, False)
        self._this = <canvas_base_t*> new canvas_g8_t(&image[0][0],
                                                      image.shape[1],
                                                      image.shape[0],
                                                      image.strides[0], 1)
