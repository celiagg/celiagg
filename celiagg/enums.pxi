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

cpdef enum PixelFormat:
    Gray8 = _enums.k_PixelFormatGray8
    Gray16 = _enums.k_PixelFormatGray16
    Gray32 = _enums.k_PixelFormatGray32
    BGR24 = _enums.k_PixelFormatBGR24
    RGB24 = _enums.k_PixelFormatRGB24
    BGR48 = _enums.k_PixelFormatBGR48
    RGB48 = _enums.k_PixelFormatRGB48
    BGR96 = _enums.k_PixelFormatBGR96
    RGB96 = _enums.k_PixelFormatRGB96
    BGRA32 = _enums.k_PixelFormatBGRA32
    RGBA32 = _enums.k_PixelFormatRGBA32
    ARGB32 = _enums.k_PixelFormatARGB32
    ABGR32 = _enums.k_PixelFormatABGR32
    BGRA64 = _enums.k_PixelFormatBGRA64
    RGBA64 = _enums.k_PixelFormatRGBA64
    ARGB64 = _enums.k_PixelFormatARGB64
    ABGR64 = _enums.k_PixelFormatABGR64
    BGRA128 = _enums.k_PixelFormatBGRA128
    RGBA128 = _enums.k_PixelFormatRGBA128
    ARGB128 = _enums.k_PixelFormatARGB128
    ABGR128 = _enums.k_PixelFormatABGR128

cpdef enum InnerJoin:
    InnerBevel = _enums.InnerBevel
    InnerMiter = _enums.InnerMiter
    InnerJag = _enums.InnerJag
    InnerRound = _enums.InnerRound

cpdef enum LineJoin:
    JoinMiter = _enums.JoinMiter
    JoinRound = _enums.JoinRound
    JoinBevel = _enums.JoinBevel

cpdef enum LineCap:
    CapButt = _enums.CapButt
    CapSquare = _enums.CapSquare
    CapRound = _enums.CapRound

cpdef enum DrawingMode:
    DrawFill = _enums.DrawFill
    DrawEofFill = _enums.DrawEofFill
    DrawStroke = _enums.DrawStroke
    DrawFillStroke = _enums.DrawFillStroke
    DrawEofFillStroke = _enums.DrawEofFillStroke

cpdef enum TextDrawingMode:
    TextDrawInvisible = _enums.TextDrawInvisible
    TextDrawFill = _enums.TextDrawFill
    TextDrawStroke = _enums.TextDrawStroke
    TextDrawClip = _enums.TextDrawClip
    TextDrawRaster = _enums.TextDrawRaster
    TextDrawFillStroke = _enums.TextDrawFillStroke
    TextDrawFillClip = _enums.TextDrawFillClip
    TextDrawStrokeClip = _enums.TextDrawStrokeClip
    TextDrawFillStrokeClip = _enums.TextDrawFillStrokeClip

cpdef enum BlendMode:
    BlendAlpha = _enums.BlendAlpha
    BlendClear = _enums.BlendClear
    BlendSrc = _enums.BlendSrc
    BlendDst = _enums.BlendDst
    BlendSrcOver = _enums.BlendSrcOver
    BlendDstOver = _enums.BlendDstOver
    BlendSrcIn = _enums.BlendSrcIn
    BlendDstIn = _enums.BlendDstIn
    BlendSrcOut = _enums.BlendSrcOut
    BlendDstOut = _enums.BlendDstOut
    BlendSrcAtop = _enums.BlendSrcAtop
    BlendDstAtop = _enums.BlendDstAtop
    BlendXor = _enums.BlendXor
    BlendAdd = _enums.BlendAdd
    BlendMultiply = _enums.BlendMultiply
    BlendScreen = _enums.BlendScreen
    BlendOverlay = _enums.BlendOverlay
    BlendDarken = _enums.BlendDarken
    BlendLighten = _enums.BlendLighten
    BlendColorDodge = _enums.BlendColorDodge
    BlendColorBurn = _enums.BlendColorBurn
    BlendHardLight = _enums.BlendHardLight
    BlendSoftLight = _enums.BlendSoftLight
    BlendDifference = _enums.BlendDifference
    BlendExclusion = _enums.BlendExclusion
