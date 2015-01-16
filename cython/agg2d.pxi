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
    cpdef enum LineCap:
        CapButt
        CapSquare
        CapRound
    cpdef enum TextAlignment:
        AlignLeft
        AlignRight
        AlignCenter
        AlignTop
        AlignBottom
    cpdef enum DrawPathFlag:
        FillOnly
        StrokeOnly
        FillAndStroke
        FillWithLineColor
    cpdef enum ViewportOption:
        Anisotropic
        XMinYMin
        XMidYMin
        XMaxYMin
        XMinYMid
        XMidYMid
        XMaxYMid
        XMinYMax
        XMidYMax
        XMaxYMax
    cpdef enum ImageFilter:
        NoFilter
        Bilinear
        Hanning
        Hermite
        Quadric
        Bicubic
        Catrom
        Spline16
        Spline36
        Blackman144
    cpdef enum ImageResample:
        NoResample
        ResampleAlways
        ResampleOnZoomOut
    cpdef enum FontCacheType:
        RasterFontCache
        VectorFontCache
    cpdef enum BlendMode:
        BlendAlpha
        BlendClear
        BlendSrc
        BlendDst
        BlendSrcOver
        BlendDstOver
        BlendSrcIn
        BlendDstIn
        BlendSrcOut
        BlendDstOut
        BlendSrcAtop
        BlendDstAtop
        BlendXor
        BlendAdd
        BlendMultiply
        BlendScreen
        BlendOverlay
        BlendDarken
        BlendLighten
        BlendColorDodge
        BlendColorBurn
        BlendHardLight
        BlendSoftLight
        BlendDifference
        BlendExclusion
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

    def setClipBox(self, x0, y0, x1, y1):
        """setClipBox(self, x0, y0, x1, y1)"""
        self._this.clipBox(x0, y0, x1, y1)

    def getClipBox(self):
        """getClipBox(self)"""
        cdef _agg2d.RectD clipBox = self._this.clipBox()
        return (clipBox.x1, clipBox.y1, clipBox.x2, clipBox.y2)

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

    def setLineWidth(self, w):
        """setLineWidth(self, w)"""
        self._this.lineWidth(w)

    def getLineWidth(self):
        """getLineWidth(self)"""
        return self._this.lineWidth()

    def setLineCap(self, LineCap cap):
        """setLineCap(self, LineCap cap)"""
        self._this.lineCap(cap)

    def getLineCap(self):
        """getLineCap(self)"""
        return self._this.lineCap()

    def setLineJoin(self, LineJoin join):
        """setLineJoin(self, LineJoin join)"""
        self._this.lineJoin(join)

    def getLineJoin(self):
        """getLineJoin(self)"""
        return self._this.lineJoin()

    def setFillEvenOdd(self, bool evenOddFlag):
        """setFillEvenOdd(self, bool evenOddFlag)"""
        self._this.fillEvenOdd(evenOddFlag)

    def getFillEvenOdd(self):
        """getFillEvenOdd(self)"""
        return self._this.fillEvenOdd()

    def line(self, x0, y0, x1, y1):
        """line(self, x0, y0, x1, y1)"""
        self._this.line(x0, y0, x1, y1)

    def triangle(self, x0, y0, x1, y1, x2, y2):
        """triangle(self, x0, y0, x1, y1, x2, y2)"""
        self._this.triangle(x0, y0, x1, y1, x2, y2)

    def rectangle(self, x0, y0, x1, y1):
        """rectangle(self, x0, y0, x1, y1)"""
        self._this.rectangle(x0, y0, x1, y1)

    def roundedRect(self, x0, y0, x1, y1,
                    r=None,
                    rx=None, ry=None,
                    rxBottom=None, ryBottom=None, rxTop=None, ryTop=None):
        """roundedRect(self, x0, y0, x1, y1,
         ............r=None,
         ............rx=None, ry=None,
         ............rxBottom=None, ryBottom=None, rxTop=None, ryTop=None):
         One of the argument sets
         -> r,
         -> rx and ry,
         -> rxBottom, ryBottom, rxTop, and ryTop
         must be suppied (ie, if r is given, rx, ry, rxBottom, ryBottom, rxTop, and ryTop must all be None)"""
        cdef uint8_t rNone = <uint8_t>(r is None)
        cdef uint8_t rxyNone = <uint8_t>(rx is None and ry is None)
        cdef uint8_t rxybtNone = <uint8_t>(rxBottom is None and ryBottom is None and rxTop is None and ryTop is None)
        print('rNone {} rxyNone {} rxybtNone {}'.format(rNone, rxyNone, rxybtNone))
        if rNone + rxyNone + rxybtNone != 2:
            raise ValueError('Either r, or rx & ry, or rxBottom & ryBottom & rxTop & ryTop must be supplied.')
        if rNone == 0:
            self._this.roundedRect(x0, y0, x1, y1, r)
        elif rxyNone == 0:
            self._this.roundedRect(x0, y0, x1, y1, rx, ry)
        elif rxybtNone == 0:
            self._this.roundedRect(x0, y0, x1, y1, rxBottom, ryBottom, rxTop, ryTop)
        else:
            raise RuntimeError('Logical inconsistency encountered during argument processing...')

    def ellipse(self, cx, cy, rx, ry):
        """ellipse(self, cx, cy, rx, ry)"""
        self._this.ellipse(cx, cy, rx, ry)

    def arc(self, cx, cy, rx, ry, start, sweep):
        """arc(self, cx, cy, rx, ry, start, sweep)"""
        self._this.arc(cx, cy, rx, ry, start, sweep)

    def star(self, cx, cy, r0, r1, startAngle, numRays):
        """star(self, cx, cy, r0, r1, startAngle, numRays)"""
        self._this.star(cx, cy, r0, r1, startAngle, numRays)

    def curve3(self, x0, y0, x1, y1, x2, y2):
        """curve3(self, x0, y0, x1, y1, x2, y2)"""
        self._this.curve(x0, y0, x1, y1, x2, y2)

    def curve4(self, x0, y0, x1, y1, x2, y2, x3, y3):
        """curve4(self, x0, y0, x1, y1, x2, y2, x3, y3)"""
        self._this.curve(x0, y0, x1, y1, x2, y2, x3, y3)

    def polygon(self, points):
        """polygon(self, points)"""
        cdef double[:,::1] points_npy = numpy.asarray(points, dtype=numpy.float64, order='c')
        if points_npy.shape[1] != 2:
            raise ValueError('points argument must be an iterable of (x, y) pairs.')
        self._this.polygon(&points_npy[0][0], points_npy.shape[0])

    def polyline(self, points):
        """polyline(self, points)"""
        cdef double[:,::1] points_npy = numpy.asarray(points, dtype=numpy.float64, order='c')
        if points_npy.shape[1] != 2:
            raise ValueError('points argument must be an iterable of (x, y) pairs.')
        self._this.polyline(&points_npy[0][0], points_npy.shape[0])

    def resetPath(self):
        """resetPath(self)"""
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
