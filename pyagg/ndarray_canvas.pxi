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

ctypedef _ndarray_canvas.ndarray_canvas_base[uint8_t] canvas_base_t
ctypedef _ndarray_canvas.ndarray_canvas[_ndarray_canvas.pixfmt_rgba32, uint8_t] canvas_rgba32_t
ctypedef _ndarray_canvas.ndarray_canvas[_ndarray_canvas.pixfmt_rgb24, uint8_t] canvas_rgb24_t
ctypedef _ndarray_canvas.ndarray_canvas[_ndarray_canvas.pixfmt_gray8, uint8_t] canvas_ga16_t

@cython.internal
cdef class ImageBase:
    cdef _ndarray_canvas.Image* _this

    def __dealloc__(self):
            del self._this

    @property
    def width(self):
        return self._this.width()

    @property
    def height(self):
        return self._this.height()


@cython.internal
cdef class ImageRGBA32(ImageBase):
    def __cinit__(self, uint8_t[:,:,::1] arr):
        self._this = new _ndarray_canvas.Image(&arr[0][0][0],
                                               arr.shape[1], arr.shape[0],
                                               arr.strides[0])


@cython.internal
cdef class ImageRGB24(ImageBase):
    def __cinit__(self, uint8_t[:,:,::1] arr):
        self._this = new _ndarray_canvas.Image(&arr[0][0][0],
                                               arr.shape[1], arr.shape[0],
                                               arr.strides[0])


@cython.internal
cdef class ImageGA16(ImageBase):
    def __cinit__(self, uint8_t[:,:,::1] arr):
        self._this = new _ndarray_canvas.Image(&arr[0][0][0],
                                               arr.shape[1], arr.shape[0],
                                               arr.strides[0])


@cython.internal
cdef class ImageG8(ImageBase):
    def __cinit__(self, uint8_t[:,::1] arr):
        self._this = new _ndarray_canvas.Image(&arr[0][0],
                                               arr.shape[1], arr.shape[0],
                                               arr.strides[0])


@cython.internal
cdef class CanvasBaseUInt8:
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

    def draw_path(self, Path path, Transform transform, GraphicsState state):
        """draw_path(self, path, state)
          path: A Path object
          transform: A Transform object
          state: A GraphicsState object
                 line width, line color, fill color, anti-aliased
        """
        self._this.draw_path(dereference(path._this),
                             dereference(transform._this),
                             dereference(state._this))

    def draw_bspline(self, points, Transform transform, GraphicsState state):
        """draw_bspline(self, points, state):
          points: Iterable of (x, y) pairs representing B-spline control points
          transform: A Transform object
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
                                dereference(transform._this),
                                dereference(state._this))

    def draw_image(self, image, Transform transform, GraphicsState state):
        """draw_image(self, image, transform, state):
          image: A 2D or 3D numpy array containing image data
          transform: A Transform object
          state: A GraphicsState object
        """
        cdef ImageBase img = self._get_image(image)
        self._this.draw_image(dereference(img._this),
                              dereference(transform._this),
                              dereference(state._this))

    def draw_text(self, text, Font font, Transform transform,
                  GraphicsState state):
        """draw_text(self, text, font, transform, state):
          text: A UTF-8 string of text to be renderered.
          font: A Font object
          transform: A Transform object
          state: A GraphicsState object
                line color, line width, fill color, drawing mode, anti-aliased
        """
        self._this.draw_text(text, dereference(font._this),
                             dereference(transform._this),
                             dereference(state._this))


cdef class CanvasRGBA32(CanvasBaseUInt8):
    """CanvasRGBA32 provides AGG (Anti-Grain Geometry) drawing routines that
    render to the numpy array passed as CanvasRGBA32's constructor argument.
    Because this array is modified in place, it must be of type numpy.uint8,
    must be C-contiguous, and must be
    MxNx4 (4 channels: red, green, blue, and alpha).
    """
    def __cinit__(self, uint8_t[:,:,::1] image):
        self.base_init(image, (255,)*4, True)
        self._this = <canvas_base_t*> new canvas_rgba32_t(&image[0][0][0],
                                                          image.shape[1],
                                                          image.shape[0],
                                                          image.strides[0], 4)

    def _get_image(self, image):
        return ImageRGBA32(image)


cdef class CanvasRGB24(CanvasBaseUInt8):
    """CanvasRGB24 provides AGG (Anti-Grain Geometry) drawing routines that
    render to the numpy array passed as CanvasRGB24's constructor argument.
    Because this array is modified in place, it must be of type numpy.uint8,
    must be C-contiguous, and must be MxNx3 (3 channels: red, green, blue).
    """
    def __cinit__(self, uint8_t[:,:,::1] image):
        self.base_init(image, (255,)*4, False)
        self._this = <canvas_base_t*> new canvas_rgb24_t(&image[0][0][0],
                                                         image.shape[1],
                                                         image.shape[0],
                                                         image.strides[0], 3)

    def _get_image(self, image):
        return ImageRGB24(image)


cdef class CanvasGA16(CanvasBaseUInt8):
    """CanvasGA16 provides AGG (Anti-Grain Geometry) drawing routines that
    render to the numpy array passed as CanvasGA16's constructor argument.
    Because this array is modified in place, it must be of type numpy.uint8,
    must be C-contiguous, and must be MxNx2 (2 channels: intensity and alpha).
    """
    def __cinit__(self, uint8_t[:,:,::1] image):
        self.base_init(image, (255,)*2, True)
        self._this = <canvas_base_t*> new canvas_ga16_t(&image[0][0][0],
                                                        image.shape[1],
                                                        image.shape[0],
                                                        image.strides[0], 2)

    def _get_image(self, image):
        return ImageGA16(image)


cdef class CanvasG8(CanvasBaseUInt8):
    """CanvasG8 provides AGG (Anti-Grain Geometry) drawing routines that render
    to the numpy array passed as CanvasG8's constructor argument.  Because this
    array is modified in place, it must be of type numpy.uint8, must be
    C-contiguous, and must be MxN (1 channel: intensity).
    """
    def __cinit__(self, uint8_t[:,::1] image):
        self.base_init(image, (255,)*2, False)
        self._this = <canvas_base_t*> new canvas_ga16_t(&image[0][0],
                                                        image.shape[1],
                                                        image.shape[0],
                                                        image.strides[0], 1)

    def _get_image(self, image):
        return ImageG8(image)
