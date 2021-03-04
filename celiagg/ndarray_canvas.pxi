# The MIT License (MIT)
#
# Copyright (c) 2014 WUSTL ZPLAB
# Copyright (c) 2016-2021 Celiagg Contributors
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

cimport numpy

ctypedef unsigned char _bytes_t

ctypedef _ndarray_canvas.ndarray_canvas_base canvas_base_t
ctypedef _ndarray_canvas.ndarray_canvas[_ndarray_canvas.pixfmt_rgba128] canvas_rgba128_t
ctypedef _ndarray_canvas.ndarray_canvas[_ndarray_canvas.pixfmt_bgra32] canvas_brga32_t
ctypedef _ndarray_canvas.ndarray_canvas[_ndarray_canvas.pixfmt_rgba32] canvas_rgba32_t
ctypedef _ndarray_canvas.ndarray_canvas[_ndarray_canvas.pixfmt_rgb24] canvas_rgb24_t
ctypedef _ndarray_canvas.ndarray_canvas[_ndarray_canvas.pixfmt_gray8] canvas_ga16_t


@cython.internal
cdef class CanvasBase:
    cdef canvas_base_t* _this
    cdef PixelFormat pixel_format
    cdef object py_array
    cdef object font_cache
    cdef bool bottom_up

    cdef int base_init(self, image, int channel_count, bool has_alpha) except -1:
        if image is None:
            raise ValueError('image argument must not be None.')

        cdef numpy.npy_int32[:] image_shape = numpy.asarray(image.shape,
                                                            dtype=numpy.int32,
                                                            order='c')
        cdef int image_ndim = <int> image.ndim

        if not has_alpha:
            # Draw colors for images without alpha channels are specified with
            # alpha channels and the drawing operations are blended as if all
            # image alpha channel pixels are at max value.  Therefore, required
            # channel count is one less than default color channel count if
            # image does not have an alpha channel.
            channel_count -= 1
        if (channel_count > 1 and (image_ndim != 3
                or image_shape[2] != channel_count) or
                channel_count == 1 and image_ndim != 2):
            if channel_count > 1:
                e = 'MxNx{}'.format(channel_count)
            else:
                e = 'MxN'
            msg_tmp = 'image argument must be {} channel (ie, must be {}).'
            raise ValueError(msg_tmp.format(channel_count, e))
        # In order to ensure that our backing memory is not deallocated from
        # underneath us, we retain a reference to the memory view supplied to
        # the constructor (image) as self.py_array, to be kept in lock step
        # with ndarray_canvas<>'s reference to that memory and destroyed when
        # that reference is lost.
        self.py_array = image

    def __dealloc__(self):
        del self._this

    property channel_count:
        def __get__(self):
            return self._this.channel_count()

    property width:
        def __get__(self):
            return self._this.width()

    property height:
        def __get__(self):
            return self._this.height()

    property array:
        def __get__(self):
            # User is not likely to be concerned with the details of the cython
            # memory view around array or buffer supplied to the constructor or
            # attach, but instead expects the original array or buffer (which
            # is why self.py_array.base is returned rather than self.py_array).
            return self.py_array.base

    property font_cache:
        def __get__(self):
            return self.font_cache

    property image:
        def __get__(self):
            return Image(self.py_array.base, self.pixel_format,
                         bottom_up=self.bottom_up)

    def clear(self, double r, double g, double b, double a=1.0):
        """clear(r, g, b, a)
        Fill the canvas with a single RGBA value

        :param r: Red value in [0, 1]
        :param g: Green value in [0, 1]
        :param b: Blue value in [0, 1]
        :param a: Alpha value in [0, 1] (defaults to 1.0)
        """
        self._this.clear(r, g, b, a)

    def draw_image(self, image, fmt, transform, state, bottom_up=False):
        """draw_image(image, format, transform, state, bottom_up=False)
        Draw an image on the canvas.

        :param image: A 2D or 3D numpy array containing image data
        :param format: A ``PixelFormat`` describing the array's data
        :param transform: A ``Transform`` object
        :param state: A ``GraphicsState`` object
        :param bottom_up: If True, the image data is flipped in the y axis
        """
        if not isinstance(image, (numpy.ndarray, Image)):
            raise TypeError("image must be an ndarray or Image instance")
        if not isinstance(fmt, PixelFormat) and isinstance(image, numpy.ndarray):
            raise TypeError("format must be a PixelFormat value")
        if not isinstance(transform, Transform):
            raise TypeError("transform must be a Transform instance")
        if not isinstance(state, GraphicsState):
            raise TypeError("state must be a GraphicsState instance")

        cdef GraphicsState gs = <GraphicsState>state
        cdef PixelFormat pix_fmt = <PixelFormat>image.format if fmt is None else fmt
        cdef Transform trans = <Transform>transform
        cdef Image img
        cdef Image input_img

        self._check_stencil(gs)

        if isinstance(image, Image):
            input_img = image
        else:
            input_img = Image(image, pix_fmt, bottom_up=bottom_up)

        img = self._get_native_image(input_img, self.pixel_format)
        self._this.draw_image(dereference(img._this),
                              dereference(trans._this),
                              dereference(gs._this))

    def draw_shape(self, shape, transform, state, stroke=None, fill=None):
        """draw_shape(shape, transform, state, stroke=SolidColor(0, 0, 0), fill=SolidColor(0, 0, 0))
        Draw a shape on the canvas.

        .. note::
           Use ``GraphicsState.drawing_mode`` to enable/disable stroke or fill
           drawing.

        :param shape: A ``VertexSource`` object
        :param transform: A ``Transform`` object
        :param state: A ``GraphicsState`` object
        :param stroke: The ``Paint`` to use for outlines. Defaults to black.
        :param fill: The ``Paint`` to use for fills. Defaults to black.
        """
        if not isinstance(shape, VertexSource):
            raise TypeError("shape must be a VertexSource (Path, BSpline, etc)")
        if not isinstance(transform, Transform):
            raise TypeError("transform must be a Transform instance")
        if not isinstance(state, GraphicsState):
            raise TypeError("state must be a GraphicsState instance")
        if stroke is not None and not isinstance(stroke, Paint):
            raise TypeError("stroke must be a Paint instance")
        if fill is not None and not isinstance(fill, Paint):
            raise TypeError("fill must be a Paint instance")

        cdef:
            VertexSource shp = <VertexSource>shape
            GraphicsState gs = <GraphicsState>state
            Transform trans = <Transform>transform
            PixelFormat fmt = self.pixel_format
            Paint stroke_paint
            Paint fill_paint

        self._check_stencil(gs)
        stroke_paint = self._get_native_paint(stroke, fmt)
        fill_paint = self._get_native_paint(fill, fmt)

        self._this.draw_shape(dereference(shp._this),
                              dereference(trans._this),
                              dereference(stroke_paint._this),
                              dereference(fill_paint._this),
                              dereference(gs._this))

    def draw_shape_at_points(self, shape, points, transform, state, stroke=None, fill=None):
        """draw_shape_at_points(shape, points, transform, state, stroke=SolidColor(0, 0, 0), fill=SolidColor(0, 0, 0))
        Draw a shape at multiple points on the canvas.

        .. note::
           Use ``GraphicsState.drawing_mode`` to enable/disable stroke or fill
           drawing.

        :param shape: A ``VertexSource`` object
        :param points: A sequence of (x, y) pairs where the ``shape`` should be drawn.
        :param transform: A ``Transform`` object
        :param state: A ``GraphicsState`` object
        :param stroke: The ``Paint`` to use for outlines. Defaults to black.
        :param fill: The ``Paint`` to use for fills. Defaults to black.
        """
        if not isinstance(shape, VertexSource):
            raise TypeError("shape must be a VertexSource (Path, BSpline, etc)")
        if not isinstance(transform, Transform):
            raise TypeError("transform must be a Transform instance")
        if not isinstance(state, GraphicsState):
            raise TypeError("state must be a GraphicsState instance")
        if stroke is not None and not isinstance(stroke, Paint):
            raise TypeError("stroke must be a Paint instance")
        if fill is not None and not isinstance(fill, Paint):
            raise TypeError("fill must be a Paint instance")

        cdef:
            VertexSource shp = <VertexSource>shape
            double[:,::1] _points = numpy.asarray(points, dtype=numpy.float64,
                                                  order='c')
            GraphicsState gs = <GraphicsState>state
            Transform trans = <Transform>transform
            PixelFormat fmt = self.pixel_format
            Paint stroke_paint
            Paint fill_paint

        if _points.shape[1] != 2:
            msg = 'Points argument must be an iterable of (x, y) pairs.'
            raise ValueError(msg)

        self._check_stencil(gs)
        stroke_paint = self._get_native_paint(stroke, fmt)
        fill_paint = self._get_native_paint(fill, fmt)

        self._this.draw_shape_at_points(dereference(shp._this),
                                        &_points[0][0], _points.shape[0],
                                        dereference(trans._this),
                                        dereference(stroke_paint._this),
                                        dereference(fill_paint._this),
                                        dereference(gs._this))

    def draw_text(self, text, font, transform, state, stroke=None, fill=None):
        """draw_text(text, font, transform, state, stroke=SolidColor(0, 0, 0), fill=SolidColor(0, 0, 0))
        Draw a line of text on the canvas.

        .. note::
           Use ``GraphicsState.drawing_mode`` to enable/disable stroke or fill
           drawing.

        :param text: A Unicode string of text to be renderered. Newlines will
                     be ignored.
        :param font: A ``Font`` object
        :param transform: A ``Transform`` object
        :param state: A ``GraphicsState`` object
        :param stroke: The ``Paint`` to use for outlines. Defaults to black.
        :param fill: The ``Paint`` to use for fills. Defaults to black.
        """
        if not _text_support._has_text_rendering():
            msg = ("The celiagg library was compiled without font support!  "
                   "If you would like to render text, you will need to "
                   "reinstall the library.")
            raise RuntimeError(msg)

        if not isinstance(font, FontBase):
            raise TypeError("font must be a Font instance")
        if not isinstance(transform, Transform):
            raise TypeError("transform must be a Transform instance")
        if not isinstance(state, GraphicsState):
            raise TypeError("state must be a GraphicsState instance")
        if stroke is not None and not isinstance(stroke, Paint):
            raise TypeError("stroke must be a Paint instance")
        if fill is not None and not isinstance(fill, Paint):
            raise TypeError("fill must be a Paint instance")

        cdef:
            FontBase fnt = <FontBase>font
            GraphicsState gs = <GraphicsState>state
            Transform trans = <Transform>transform
            PixelFormat fmt = self.pixel_format
            Paint stroke_paint
            Paint fill_paint

        self._check_stencil(gs)
        stroke_paint = self._get_native_paint(stroke, fmt)
        fill_paint = self._get_native_paint(fill, fmt)

        text = _get_utf8_text(text, "The text argument must be unicode.")
        self._this.draw_text(text, dereference(fnt._this),
                             dereference(trans._this),
                             dereference(stroke_paint._this),
                             dereference(fill_paint._this),
                             dereference(gs._this))

    cdef _check_stencil(self, GraphicsState state):
        """Internal. Checks if a stencil's dimensions match those of the
        canvas.
        """
        cdef Image stencil = state.stencil
        if stencil is not None:
            w, h = self.width, self.height
            sw, sh = stencil.width, stencil.height
            if sw != w or sh != h:
                msg = ("The stencil set on the GraphicsState object does not "
                       "have the same dimensions as the target canvas! "
                       "(({}, {}) vs. ({}, {}))").format(sw, sh, w, h)
                raise AggError(msg)

    cdef Image _get_native_image(self, Image image, PixelFormat fmt):
        """_get_native_image(image, format)

        :param image: An ``Image`` object which is needed in a different pixel
                      format.
        :param format: The desired output pixel format
        """
        if image.pixel_format == fmt:
            return image

        return convert_image(image, fmt, bottom_up=image.bottom_up)

    cdef Paint _get_native_paint(self, paint, PixelFormat fmt):
        """_get_native_paint(paint, format)

        :param paint: A ``Paint`` object which is needed in a different pixel
                      format.
        :param format: The desired output pixel format
        """
        if paint is None:
            return SolidPaint(0.0, 0.0, 0.0)

        if not hasattr(paint, '_with_format'):
            return paint

        return paint._with_format(fmt)


cdef class CanvasRGBA128(CanvasBase):
    def __cinit__(self, float[:,:,::1] image, FontCache cache, bottom_up=False):
        cdef:
            FontCache font_cache = <FontCache>cache

        self.base_init(image, 4, True)
        self.pixel_format = PixelFormat.RGBA128
        self.bottom_up = bottom_up
        self.font_cache = cache
        self._this = <canvas_base_t*> new canvas_rgba128_t(<_bytes_t*>&image[0][0][0],
                                                           image.shape[1],
                                                           image.shape[0],
                                                           image.strides[0], 4,
                                                           dereference(font_cache._this),
                                                           bottom_up)


cdef class CanvasBGRA32(CanvasBase):
    def __cinit__(self, _bytes_t[:,:,::1] image, FontCache cache, bottom_up=False):
        cdef:
            FontCache font_cache = <FontCache>cache

        self.base_init(image, 4, True)
        self.pixel_format = PixelFormat.BGRA32
        self.bottom_up = bottom_up
        self.font_cache = cache
        self._this = <canvas_base_t*> new canvas_brga32_t(&image[0][0][0],
                                                          image.shape[1],
                                                          image.shape[0],
                                                          image.strides[0], 4,
                                                          dereference(font_cache._this),
                                                          bottom_up)


cdef class CanvasRGBA32(CanvasBase):
    def __cinit__(self, _bytes_t[:,:,::1] image, FontCache cache, bottom_up=False):
        cdef:
            FontCache font_cache = <FontCache>cache

        self.base_init(image, 4, True)
        self.pixel_format = PixelFormat.RGBA32
        self.bottom_up = bottom_up
        self.font_cache = cache
        self._this = <canvas_base_t*> new canvas_rgba32_t(&image[0][0][0],
                                                          image.shape[1],
                                                          image.shape[0],
                                                          image.strides[0], 4,
                                                          dereference(font_cache._this),
                                                          bottom_up)


cdef class CanvasRGB24(CanvasBase):
    def __cinit__(self, _bytes_t[:,:,::1] image, FontCache cache, bottom_up=False):
        cdef:
            FontCache font_cache = <FontCache>cache

        self.base_init(image, 4, False)
        self.pixel_format = PixelFormat.RGB24
        self.bottom_up = bottom_up
        self.font_cache = cache
        self._this = <canvas_base_t*> new canvas_rgb24_t(&image[0][0][0],
                                                         image.shape[1],
                                                         image.shape[0],
                                                         image.strides[0], 3,
                                                         dereference(font_cache._this),
                                                         bottom_up)


cdef class CanvasGA16(CanvasBase):
    def __cinit__(self, _bytes_t[:,:,::1] image, FontCache cache, bottom_up=False):
        cdef:
            FontCache font_cache = <FontCache>cache

        self.base_init(image, 2, True)
        self.pixel_format = PixelFormat.Gray8
        self.bottom_up = bottom_up
        self.font_cache = cache
        self._this = <canvas_base_t*> new canvas_ga16_t(&image[0][0][0],
                                                        image.shape[1],
                                                        image.shape[0],
                                                        image.strides[0], 2,
                                                        dereference(font_cache._this),
                                                        bottom_up)


cdef class CanvasG8(CanvasBase):
    def __cinit__(self, _bytes_t[:,::1] image, FontCache cache, bottom_up=False):
        cdef:
            FontCache font_cache = <FontCache>cache

        self.base_init(image, 2, False)
        self.pixel_format = PixelFormat.Gray8
        self.bottom_up = bottom_up
        self.font_cache = cache
        self._this = <canvas_base_t*> new canvas_ga16_t(&image[0][0],
                                                        image.shape[1],
                                                        image.shape[0],
                                                        image.strides[0], 1,
                                                        dereference(font_cache._this),
                                                        bottom_up)
