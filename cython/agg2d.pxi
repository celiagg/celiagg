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

cdef extern from "agg2d/agg2d.h" namespace "Agg2D":
    cpdef enum LineJoin:
        JoinMiter
        JoinRound
        JoinBevel
    cpdef enum DrawPathFlag:
        FillOnly
        StrokeOnly
        FillAndStroke
        FillWithLineColor
    cpdef enum FontCacheType:
        RasterFontCache
        VectorFontCache
    cpdef enum Direction:
        CW
        CCW

cdef class Image:
    cdef _agg2d.Image* _this
    cdef object py_buffer

    def __cinit__(self, uint8_t[:,:,::1] buffer):
        self._this = new _agg2d.Image(&buffer[0][0][0], buffer.shape[1], buffer.shape[0], buffer.strides[0])
        self.py_buffer = buffer

    def __dealloc__(self):
        del self._this

    def attach(self, uint8_t[:,:,::1] buffer):
        self._this.attach(&buffer[0][0][0], buffer.shape[1], buffer.shape[0], buffer.strides[0])
        self.py_buffer = buffer

    @property
    def buffer(self):
        # User is not likely to be concerned with the details of the cython memory view around array or buffer
        # supplied to the constructor or attach, but instead expects the original array or buffer (which is why
        # self.py_buffer.base is returned rather than self.py_buffer).
        return self.py_buffer.base

cdef class Agg2D:
    cdef _agg2d.Agg2D* _this
    cdef Image _image

    def __cinit__(self, uint8_t[:,:,::1] buffer):
        self._this = new _agg2d.Agg2D()
        self._image = Image(buffer)
        self._this.attach(dereference(self._image._this))

    def __dealloc__(self):
        del self._this

    @property
    def image(self):
        return self._image

    @property
    def buffer(self):
        return self._image.buffer

    def setFillColor(self, uint8_t r, uint8_t g, uint8_t b, uint8_t a=255):
        """setFillColor(self, uint8_t r, uint8_t g, uint8_t b, uint8_t a=255)"""
        self._this.fillColor(r, g, b, a)

    def noFill(self):
        """noFill(self)"""
        self._this.noFill()

    def setLineColor(self, uint8_t r, uint8_t g, uint8_t b, uint8_t a=255):
        """setLineColor(self, uint8_t r, uint8_t g, uint8_t b, uint8_t a=255)"""
        self._this.lineColor(r, g, b, a)

    def noLine(self):
        """noLine(self)"""
        self._this.noLine()

    def getFillColor(self):
        """getFillColor(self)"""
        cdef _agg2d.srgba8 ret = self._this.fillColor()
        return (ret.r, ret.g, ret.b, ret.a)

    def getLineColor(self):
        """getLineColor(self)"""
        cdef _agg2d.srgba8 ret = self._this.lineColor()
        return (ret.r, ret.g, ret.b, ret.a)

    def resetPath(self):
        self._this.resetPath()

    def addEllipse(self, cx, cy, rx, ry, Direction dir_=CW):
        """addEllipse(self, cx, cy, rx, ry, Direction dir_=CW)"""
        self._this.addEllipse(cx, cy, rx, ry, dir_)

    def closePolygon(self):
        """closePolygon(self)"""
        self._this.closePolygon()

    def setFlipText(self, bool flip):
        """setFlipText(self, bool flip)"""
        self._this.flipText(flip)

    def setFont(self, str fileName, height, bool bold=False, bool italic=False, FontCacheType ch=RasterFontCache, angle=0.0):
        """setFont(self, str fileName, height, bool bold=False, bool italic=False, FontCacheType ch=RasterFontCache, angle=0.0):
         ex: agg2d.setFont('/Library/Fonts/Times New Roman.ttf', 50)"""
        self._this.font(fileName.encode('UTF-8'), height, bold, italic, ch, angle)

    def getFontHeight(self):
        """getFontHeight(self)"""
        return self._this.fontHeight()

    def getTextHints(self):
        """getTextHints(self)"""
        return self._this.textHints()

    def setTextHints(self, bool hints):
        """setTextHints(self, bool hints)"""
        self._this.textHints(hints)

    def getTextWidth(self, str s):
        """getTextWidth(self, str s)"""
        return self._this.textWidth(s.encode('UTF-8'))

    def text(self, x, y, str s, bool roundOff=False, double dx=0, double dy=0):
        """text(self, x, y, str s, bool roundOff=False, double dx=0, double dy=0)"""
        self._this.text(x, y, s.encode('UTF-8'), roundOff, dx, dy)

    def drawPath(self, DrawPathFlag flag=FillAndStroke):
        """drawPath(self, DrawPathFlag flag=FillAndStroke)"""
        self._this.drawPath(flag)

#   def drawPathNoTransform(self, DrawPathFlag flag = FillAndStroke):
#       self._this.drawPathNoTransform(flag)
