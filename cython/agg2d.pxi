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

class LineJoin(IntEnum):
    JoinMiter = _agg2d.JoinMiter
    JoinRound = _agg2d.JoinRound
    JoinBevel = _agg2d.JoinBevel
class LineCap(IntEnum):
    CapButt = _agg2d.CapButt
    CapSquare = _agg2d.CapSquare
    CapRound = _agg2d.CapRound
class TextAlignment(IntEnum):
    AlignLeft = _agg2d.AlignLeft
    AlignRight = _agg2d.AlignRight
    AlignCenter = _agg2d.AlignCenter
    AlignTop = _agg2d.AlignTop
    AlignBottom = _agg2d.AlignBottom
class DrawPathFlag(IntEnum):
    FillOnly = _agg2d.FillOnly
    StrokeOnly = _agg2d.StrokeOnly
    FillAndStroke = _agg2d.FillAndStroke
    FillWithLineColor = _agg2d.FillWithLineColor
class ViewportOption(IntEnum):
    Anisotropic = _agg2d.Anisotropic
    XMinYMin = _agg2d.XMinYMin
    XMidYMin = _agg2d.XMidYMin
    XMaxYMin = _agg2d.XMaxYMin
    XMinYMid = _agg2d.XMinYMid
    XMidYMid = _agg2d.XMidYMid
    XMaxYMid = _agg2d.XMaxYMid
    XMinYMax = _agg2d.XMinYMax
    XMidYMax = _agg2d.XMidYMax
    XMaxYMax = _agg2d.XMaxYMax
class ImageFilter(IntEnum):
    NoFilter = _agg2d.NoFilter
    Bilinear = _agg2d.Bilinear
    Hanning = _agg2d.Hanning
    Hermite = _agg2d.Hermite
    Quadric = _agg2d.Quadric
    Bicubic = _agg2d.Bicubic
    Catrom = _agg2d.Catrom
    Spline16 = _agg2d.Spline16
    Spline36 = _agg2d.Spline36
    Blackman144 = _agg2d.Blackman144
class ImageResample(IntEnum):
    NoResample = _agg2d.NoResample
    ResampleAlways = _agg2d.ResampleAlways
    ResampleOnZoomOut = _agg2d.ResampleOnZoomOut
class FontCacheType(IntEnum):
    RasterFontCache = _agg2d.RasterFontCache
    VectorFontCache = _agg2d.VectorFontCache
class BlendMode(IntEnum):
    BlendAlpha = _agg2d.BlendAlpha
    BlendClear = _agg2d.BlendClear
    BlendSrc = _agg2d.BlendSrc
    BlendDst = _agg2d.BlendDst
    BlendSrcOver = _agg2d.BlendSrcOver
    BlendDstOver = _agg2d.BlendDstOver
    BlendSrcIn = _agg2d.BlendSrcIn
    BlendDstIn = _agg2d.BlendDstIn
    BlendSrcOut = _agg2d.BlendSrcOut
    BlendDstOut = _agg2d.BlendDstOut
    BlendSrcAtop = _agg2d.BlendSrcAtop
    BlendDstAtop = _agg2d.BlendDstAtop
    BlendXor = _agg2d.BlendXor
    BlendAdd = _agg2d.BlendAdd
    BlendMultiply = _agg2d.BlendMultiply
    BlendScreen = _agg2d.BlendScreen
    BlendOverlay = _agg2d.BlendOverlay
    BlendDarken = _agg2d.BlendDarken
    BlendLighten = _agg2d.BlendLighten
    BlendColorDodge = _agg2d.BlendColorDodge
    BlendColorBurn = _agg2d.BlendColorBurn
    BlendHardLight = _agg2d.BlendHardLight
    BlendSoftLight = _agg2d.BlendSoftLight
    BlendDifference = _agg2d.BlendDifference
    BlendExclusion = _agg2d.BlendExclusion
class Direction(IntEnum):
    CW = _agg2d.CW
    CCW = _agg2d.CCW

cdef class Image:
    cdef _agg2d.Image* _this
    cdef object py_buffer

    def __cinit__(self, uint8_t[:,:,::1] buffer):
        self._this = new _agg2d.Image(&buffer[0][0][0], buffer.shape[1], buffer.shape[0], buffer.strides[0])
        self.py_buffer = buffer
        self.text_hints.__doc

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

    property image:
        def __get__(self):
            return self._image

    property buffer:
        def __get__(self):
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

    property line_width:
        def __get__(self):
            return self._this.lineWidth()
        def __set__(self, w):
            self._this.lineWidth(w)

    property line_cap:
        def __get__(self):
            return LineCap(self._this.lineCap())
        def __set__(self, cap):
            cap = LineCap(cap)
            self._this.lineCap(cap)

    property line_join:
        def __get__(self):
            return LineJoin(self._this.lineJoin())
        def __set__(self, join):
            join = LineJoin(join)
            self._this.lineJoin(join.value)

    property fill_even_odd:
        def __get__(self):
            return self._this.fillEvenOdd()
        def __set__(self, even_odd_flag):
            self._this.fillEvenOdd(even_odd_flag)

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

    def addEllipse(self, cx, cy, rx, ry, dir_=Direction.CW):
        """addEllipse(self, cx, cy, rx, ry, dir_=Direction.CW)"""
        dir_ = Direction(dir_)
        self._this.addEllipse(cx, cy, rx, ry, dir_)

    def closePolygon(self):
        """closePolygon(self)"""
        self._this.closePolygon()

    property flip_text:
        def __get__(self):
            return self._this.flipText()
        def __set__(self, flip):
            self._this.flipText(flip)

    def setFont(self, str fileName, height, bool bold=False, bool italic=False, ch=FontCacheType.RasterFontCache, angle=0.0):
        """setFont(self, str fileName, height, bool bold=False, bool italic=False, ch=FontCacheType.RasterFontCache, angle=0.0):
         ex: agg2d.setFont('/Library/Fonts/Times New Roman.ttf', 50)"""
        ch = FontCacheType(ch)
        self._this.font(fileName.encode('UTF-8'), height, bold, italic, ch, angle)

    property font_height:
        def __get__(self):
            return self._this.fontHeight()

    property text_h_alignment:
        def __get__(self):
            return TextAlignment(self._this.textHAlignment())
        def __set__(self, h_alignment):
            h_alignment = TextAlignment(h_alignment)
            self._this.textHAlignment(h_alignment)

    property text_v_alignment:
        def __get__(self):
            return TextAlignment(self._this.textVAlignment())
        def __set__(self, v_alignment):
            v_alignment = TextAlignment(v_alignment)
            self._this.textVAlignment(v_alignment)

    property text_hints:
        '''Enable or disable text subpixel hinting.'''
        def __get__(self):
            return self._this.textHints()
        def __set__(self, hints):
            self._this.textHints(hints)

    def compute_width_of_text(self, str s):
        """compute_width_of_text(self, str s)"""
        return self._this.textWidth(s.encode('UTF-8'))

    def text(self, x, y, str s, bool roundOff=False, double dx=0, double dy=0):
        """text(self, x, y, str s, bool roundOff=False, double dx=0, double dy=0)"""
        self._this.text(x, y, s.encode('UTF-8'), roundOff, dx, dy)

    def draw_path(self, flag=DrawPathFlag.FillAndStroke):
        """draw_path(self, DrawPathFlag flag=FillAndStroke)"""
        flag = DrawPathFlag(flag)
        self._this.drawPath(flag)

#   def drawPathNoTransform(self, DrawPathFlag flag = FillAndStroke):
#       self._this.drawPathNoTransform(flag)
