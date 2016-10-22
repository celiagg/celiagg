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

from types import GetSetDescriptorType
from libcpp.vector cimport vector


cdef class Rect:
    cdef _graphics_state.Rect* _this

    def __cinit__(self, double x1=0., double y1=0., double x2=0., double y2=0.):
        self._this = new _graphics_state.Rect(x1, y1, x2, y2)

    def __dealloc__(self):
        del self._this

    def __repr__(self):
        name = type(self).__name__
        return "{}({}, {}, {}, {})".format(name, self._this.x1, self._this.y1,
                                           self._this.x2, self._this.y2)

    def __richcmp__(Rect self, Rect other, int op):
        if op == 2:  # ==
            return (self._this.x1 == other._this.x1 and
                    self._this.x2 == other._this.x2 and
                    self._this.y1 == other._this.y1 and
                    self._this.y2 == other._this.y2)
        else:
            msg = "That type of comparison is not implemented for Rect"
            raise NotImplementedError(msg)

    property x1:
        def __get__(self):
            return self._this.x1
        def __set__(self, value):
            self._this.x1 = value

    property x2:
        def __get__(self):
            return self._this.x2
        def __set__(self, value):
            self._this.x2 = value

    property y1:
        def __get__(self):
            return self._this.y1
        def __set__(self, value):
            self._this.y1 = value

    property y2:
        def __get__(self):
            return self._this.y2
        def __set__(self, value):
            self._this.y2 = value

    property valid:
        def __get__(self):
            return self._this.is_valid()


cdef class GraphicsState:
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
        """ Return a deep copy of the object
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
            return self._this.antiAliased()

        def __set__(self, aa):
            self._this.antiAliased(aa)

    property clip_box:
        def __get__(self):
            cdef _graphics_state.Rect ret = self._this.clipBox()
            return Rect(ret.x1, ret.y1, ret.x2, ret.y2)

        def __set__(self, Rect box):
            self._this.clipBox(box._this[0])

    property drawing_mode:
        def __get__(self):
            return DrawingMode(self._this.drawingMode())

        def __set__(self, DrawingMode m):
            self._this.drawingMode(m)

    property blend_mode:
        def __get__(self):
            return BlendMode(self._this.blendMode())

        def __set__(self, BlendMode m):
            self._this.blendMode(m)

    property image_blend_mode:
        def __get__(self):
            return BlendMode(self._this.imageBlendMode())

        def __set__(self, BlendMode m):
            self._this.imageBlendMode(m)

    property master_alpha:
        def __get__(self):
            return self._this.masterAlpha()

        def __set__(self, a):
            self._this.masterAlpha(a)

    property anti_alias_gamma:
        def __get__(self):
            return self._this.antiAliasGamma()

        def __set__(self, g):
            self._this.antiAliasGamma(g)

    property line_width:
        def __get__(self):
            return self._this.lineWidth()

        def __set__(self, w):
            self._this.lineWidth(w)

    property line_cap:
        def __get__(self):
            return LineCap(self._this.lineCap())

        def __set__(self, LineCap cap):
            self._this.lineCap(cap)

    property line_join:
        def __get__(self):
            return LineJoin(self._this.lineJoin())

        def __set__(self, LineJoin join):
            self._this.lineJoin(join)

    property line_dash_pattern:
        def __get__(self):
            cdef vector[double] dashes = self._this.lineDashPattern()
            return list(zip(dashes[:-1:2], dashes[1::2]))

        def __set__(self, dashes):
            if len(dashes) == 0:
                self._this.lineDashPattern(<const double *>0, 0)
                return

            cdef double[:,::1] _dashes = numpy.asarray(dashes,
                                                       dtype=numpy.float64,
                                                       order='c')
            if _dashes.shape[1] != 2:
                msg = 'Dashes must be an iterable of (dash len, gap len) pairs.'
                raise ValueError(msg)
            self._this.lineDashPattern(&_dashes[0][0], _dashes.shape[0])

    property line_dash_phase:
        def __get__(self):
            return self._this.lineDashPhase()

        def __set__(self, double phase):
            self._this.lineDashPhase(phase)

    property stencil:
        def __get__(self):
            return self._stencil_img

        def __set__(self, Image img):
            self._stencil_img = img
            if self._stencil_img is not None:
                if img.format != PixelFormat.Gray8:
                    raise ValueError("Stencil Images must have format Gray8!")
                self._this.stencil(img._this)
            else:
                self._this.stencil(<_image.Image*>0)
