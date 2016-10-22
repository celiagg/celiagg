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

cdef extern from "font.h" namespace "Font":
    cdef enum FontCacheType:
        RasterFontCache
        VectorFontCache


cdef extern from "image.h":
    cdef enum PixelFormat:
        k_PixelFormatGray8
        k_PixelFormatGray16
        k_PixelFormatGray32
        k_PixelFormatBGR24
        k_PixelFormatRGB24
        k_PixelFormatBGR48
        k_PixelFormatRGB48
        k_PixelFormatBGR96
        k_PixelFormatRGB96
        k_PixelFormatBGRA32
        k_PixelFormatRGBA32
        k_PixelFormatARGB32
        k_PixelFormatABGR32
        k_PixelFormatBGRA64
        k_PixelFormatRGBA64
        k_PixelFormatARGB64
        k_PixelFormatABGR64
        k_PixelFormatBGRA128
        k_PixelFormatRGBA128
        k_PixelFormatARGB128
        k_PixelFormatABGR128


cdef extern from "graphics_state.h" namespace "GraphicsState":
    cdef enum LineJoin:
        JoinMiter
        JoinRound
        JoinBevel

    cdef enum LineCap:
        CapButt
        CapSquare
        CapRound

    cdef enum DrawingMode:
        DrawFill
        DrawEofFill
        DrawStroke
        DrawFillStroke
        DrawEofFillStroke

    cdef enum BlendMode:
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


cdef extern from "paint.h" namespace "Paint":
    cdef enum PaintType:
        k_PaintTypeSolid
        k_PaintTypeLinearGradient
        k_PaintTypeRadialGradient
        k_PaintTypePattern

    cdef enum GradientSpread:
        k_GradientSpreadInvalid
        k_GradientSpreadPad
        k_GradientSpreadReflect
        k_GradientSpreadRepeat

    cdef enum GradientUnits:
        k_GradientUnitsInvalid
        k_GradientUnitsUserSpace
        k_GradientUnitsObjectBoundingBox

    cdef enum PatternStyle:
        k_PatternStyleRepeat
        k_PatternStyleReflect
