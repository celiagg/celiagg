# The MIT License (MIT)
#
# Copyright (c) 2016 WUSTL ZPLAB
# Copyright (c) 2016-2023 Celiagg Contributors
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

from types import GetSetDescriptorType
from libcpp.vector cimport vector


cdef class Rect:
    """Rect(x, y, w, h)
    A rectangle.

    :param x: The X coordinate of the top left corner
    :param y: The Y coordinate of the top left corner
    :param w: The width of the rectangle in pixels
    :param h: The height of the rectangle in pixels
    """
    cdef _graphics_state.Rect* _this

    def __cinit__(self, double x=0., double y=0., double w=0., double h=0.):
        self._this = new _graphics_state.Rect(x, y, x + w, y + h)

    def __dealloc__(self):
        del self._this

    def __repr__(self):
        name = type(self).__name__
        width = self._this.x2 - self._this.x1
        height = self._this.y2 - self._this.y1
        return "{}({}, {}, {}, {})".format(name, self._this.x1, self._this.y1,
                                           width, height)

    def __richcmp__(Rect self, Rect other, int op):
        if op == 2:  # ==
            return (self._this.x1 == other._this.x1 and
                    self._this.x2 == other._this.x2 and
                    self._this.y1 == other._this.y1 and
                    self._this.y2 == other._this.y2)
        else:
            msg = "That type of comparison is not implemented for Rect"
            raise NotImplementedError(msg)

    property x:
        def __get__(self):
            return self._this.x1
        def __set__(self, value):
            self._this.x1 = value

    property w:
        def __get__(self):
            return self._this.x2 - self._this.x1
        def __set__(self, value):
            self._this.x2 = self._this.x1 + value

    property y:
        def __get__(self):
            return self._this.y1
        def __set__(self, value):
            self._this.y1 = value

    property h:
        def __get__(self):
            return self._this.y2 - self._this.y1
        def __set__(self, value):
            self._this.y2 = self._this.y1 + value

    property valid:
        def __get__(self):
            return self._this.is_valid()


cdef class GraphicsState:
    """GraphicsState(**properties)
    A container for drawing state. Can be initialized using keyword
    arguments and works as a namespace for the following properties:

    * anti_aliased: A boolean denoting whether drawing is anti-aliased or not.
    * drawing_mode: A ``DrawingMode`` value denoting the drawing mode.
    * text_drawing_mode: A ``TextDrawingMode`` value denoting the text drawing
                         mode.
    * blend_mode: A ``BlendMode`` for non-image drawing. (ignored)
    * image_blend_mode: A ``BlendMode`` for image drawing. (ignored)
    * image_interpolation_moded: An ``InterpolationMode`` for image drawing.
    * line_cap: A ``LineCap`` value denoting the style of line ends.
    * line_join: A ``LineJoin`` value denoting the style of joins.
    * inner_join: An ``InnerJoin`` value denoting the style of inner joins.
    * miter_limit: The miter limit
    * inner_miter_limit: The miter limit for inner joins
    * master_alpha: A master opacity value.
    * line_width: The width when stroking lines.
    * clip_box: A ``Rect`` which defines a simple clipping area.
    * line_dash_pattern: A sequence of (dash length, gap length) pairs.
    * line_dash_phase: Where in ``line_dash_pattern`` to start, when drawing.
    * stencil: An ``Image`` with format ``Gray8`` which will mask any drawing.
               The dimensions must match the size of the canvas that is being
               drawn to.
    """
    cdef _graphics_state.GraphicsState* _this
    cdef Image _stencil_img

    def __cinit__(self):
        self._this = new _graphics_state.GraphicsState()
        self._stencil_img = None

    def __dealloc__(self):
        del self._this

    def __init__(self, **kwargs):
        properties = self._propnames()
        for key, value in kwargs.items():
            if key in properties:
                setattr(self, key, value)

    cdef list _propnames(self):
        return [name for name in GraphicsState.__dict__.keys()
                if isinstance(getattr(GraphicsState, name),
                              GetSetDescriptorType)]

    def copy(self):
        """Return a deep copy of the object
        """
        cpy = GraphicsState()
        properties = self._propnames()
        properties.remove("stencil")

        if self._stencil_img is not None:
            cpy.stencil = self._stencil_img.copy()

        for name in properties:
            value = getattr(self, name)
            setattr(cpy, name, value)

        return cpy

    property anti_aliased:
        def __get__(self):
            return self._this.anti_aliased()

        def __set__(self, aa):
            self._this.anti_aliased(aa)

    property clip_box:
        def __get__(self):
            cdef _graphics_state.Rect ret = self._this.clip_box()
            return Rect(ret.x1, ret.y1, ret.x2 - ret.x1, ret.y2 - ret.y1)

        def __set__(self, Rect box):
            if not isinstance(box, Rect):
                raise TypeError("The clip_box property must be a Rect")

            cdef Rect rect = <Rect>box
            self._this.clip_box(dereference(rect._this))

    property drawing_mode:
        def __get__(self):
            return DrawingMode(self._this.drawing_mode())

        def __set__(self, DrawingMode m):
            self._this.drawing_mode(m)

    property text_drawing_mode:
        def __get__(self):
            return TextDrawingMode(self._this.text_drawing_mode())

        def __set__(self, TextDrawingMode m):
            self._this.text_drawing_mode(m)

    property blend_mode:
        def __get__(self):
            return BlendMode(self._this.blend_mode())

        def __set__(self, BlendMode m):
            self._this.blend_mode(m)

    property image_blend_mode:
        def __get__(self):
            return BlendMode(self._this.image_blend_mode())

        def __set__(self, BlendMode m):
            self._this.image_blend_mode(m)

    property image_interpolation_mode:
        def __get__(self):
            return InterpolationMode(self._this.image_interpolation_mode())

        def __set__(self, InterpolationMode m):
            self._this.image_interpolation_mode(m)

    property master_alpha:
        def __get__(self):
            return self._this.master_alpha()

        def __set__(self, a):
            self._this.master_alpha(a)

    property line_width:
        def __get__(self):
            return self._this.line_width()

        def __set__(self, w):
            self._this.line_width(w)

    property line_cap:
        def __get__(self):
            return LineCap(self._this.line_cap())

        def __set__(self, LineCap cap):
            self._this.line_cap(cap)

    property line_join:
        def __get__(self):
            return LineJoin(self._this.line_join())

        def __set__(self, LineJoin join):
            self._this.line_join(join)

    property inner_join:
        def __get__(self):
            return InnerJoin(self._this.inner_join())

        def __set__(self, InnerJoin join):
            self._this.inner_join(join)

    property miter_limit:
        def __get__(self):
            return self._this.miter_limit()

        def __set__(self, double limit):
            self._this.miter_limit(limit)

    property inner_miter_limit:
        def __get__(self):
            return self._this.inner_miter_limit()

        def __set__(self, double limit):
            self._this.inner_miter_limit(limit)

    property line_dash_pattern:
        def __get__(self):
            cdef vector[double] dashes = self._this.line_dash_pattern()
            return list(zip(dashes[:-1:2], dashes[1::2]))

        def __set__(self, dashes):
            if len(dashes) == 0:
                self._this.line_dash_pattern(<const double *>0, 0)
                return

            cdef double[:,::1] _dashes = numpy.asarray(dashes,
                                                       dtype=numpy.float64,
                                                       order='c')
            if _dashes.shape[1] != 2:
                msg = 'Dashes must be an iterable of (dash len, gap len) pairs.'
                raise ValueError(msg)
            self._this.line_dash_pattern(&_dashes[0][0], _dashes.shape[0])

    property line_dash_phase:
        def __get__(self):
            return self._this.line_dash_phase()

        def __set__(self, double phase):
            self._this.line_dash_phase(phase)

    property stencil:
        def __get__(self):
            return self._stencil_img

        def __set__(self, img):
            if img is not None and not isinstance(img, Image):
                raise TypeError("The stencil property must be an Image")

            cdef Image image = <Image>img
            self._stencil_img = image
            if self._stencil_img is not None:
                if image.format != PixelFormat.Gray8:
                    raise ValueError("Stencil Images must have format Gray8!")
                self._this.stencil(image._this)
            else:
                self._this.stencil(<_image.Image*>0)
