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

import cython
cimport numpy
import numpy
from libc.stdint cimport uint8_t
from libcpp cimport bool

cdef extern from "agg_color_rgba.h" namespace "agg":
    cdef cppclass srgba8:
        uint8_t r
        uint8_t g
        uint8_t b
        uint8_t a
        srgba8()
        srgba8(unsigned r_, unsigned g_, unsigned b_, unsigned a_)

cdef extern from "agg2d/agg2d.h" namespace "Agg2D":
    cdef cppclass Image:
        Image()
        Image(unsigned char* buf, unsigned width, unsigned height, int stride)
        void attach(unsigned char* buf, unsigned width, unsigned height, int stride)
        int width() const
        int height() const
        void premultiply()
        void demultiply()
    cdef enum LineJoin:
        JoinMiter
        JoinRound
        JoinBevel
    cdef enum DrawPathFlag:
        FillOnly
        StrokeOnly
        FillAndStroke
        FillWithLineColor
    cdef enum FontCacheType:
        RasterFontCache
        VectorFontCache
    cdef enum Direction:
        CW
        CCW

cdef extern from "agg2d/agg2d.h":
    cdef cppclass Agg2D:
        Agg2D()
        void attach(unsigned char* buf, unsigned width, unsigned height, int stride)
        void attach(Image& img)

        void fillColor(unsigned r, unsigned g, unsigned b, unsigned a)
        void noFill()

        void lineColor(unsigned r, unsigned g, unsigned b, unsigned a)
        void noLine()

        srgba8 fillColor() const
        srgba8 lineColor() const

        void resetPath()
        void addEllipse(double cx, double cy, double rx, double ry, Direction dir)
        void closePolygon()

        void flipText(bool flip)
        void font(const char* fileName, double height, bool bold, bool italic, FontCacheType ch, double angle);
        double fontHeight() const
        bool textHints() const
        void textHints(bool hints)
        double textWidth(const char* str)
        void text(double x, double y, const char* str, bool roundOff, double dx, double dy)

        void drawPath(DrawPathFlag flag)
        void drawPathNoTransform(DrawPathFlag flag)

#   enum LineJoin
#   {
#       JoinMiter = agg::miter_join,
#       JoinRound = agg::round_join,
#       JoinBevel = agg::bevel_join
#   };
#
#   enum LineCap
#   {
#       CapButt   = agg::butt_cap,
#       CapSquare = agg::square_cap,
#       CapRound  = agg::round_cap
#   };
#
#   enum TextAlignment
#   {
#       AlignLeft,
#       AlignRight,
#       AlignCenter,
#       AlignTop = AlignRight,
#       AlignBottom = AlignLeft
#   };
#
#
#   enum DrawPathFlag
#   {
#       FillOnly,
#       StrokeOnly,
#       FillAndStroke,
#       FillWithLineColor
#   };
#
#   enum ViewportOption
#   {
#       Anisotropic,
#       XMinYMin,
#       XMidYMin,
#       XMaxYMin,
#       XMinYMid,
#       XMidYMid,
#       XMaxYMid,
#       XMinYMax,
#       XMidYMax,
#       XMaxYMax
#   };
#
#   struct Transformations
#   {
#       double affineMatrix[6];
#   };
#
#
#   struct Image
#   {
#       agg::rendering_buffer renBuf;
#
#       Image() {}
#       Image(unsigned char* buf, unsigned width, unsigned height, int stride) :
#           renBuf(buf, width, height, stride) {}
#       void attach(unsigned char* buf, unsigned width, unsigned height, int stride)
#       {
#           renBuf.attach(buf, width, height, stride);
#       }
#       int width()  const { return renBuf.width(); }
#       int height() const { return renBuf.height(); }
#       void premultiply();
#       void demultiply();
#   };
#
#   enum ImageFilter
#   {
#       NoFilter,
#       Bilinear,
#       Hanning,
#       Hermite,
#       Quadric,
#       Bicubic,
#       Catrom,
#       Spline16,
#       Spline36,
#       Blackman144
#   };
#
#   enum ImageResample
#   {
#       NoResample,
#       ResampleAlways,
#       ResampleOnZoomOut
#   };
#
#   enum FontCacheType
#   {
#       RasterFontCache,
#       VectorFontCache
#   };
#
#   enum BlendMode
#   {
#       BlendAlpha      = agg::end_of_comp_op_e,
#       BlendClear      = agg::comp_op_clear,
#       BlendSrc        = agg::comp_op_src,
#       BlendDst        = agg::comp_op_dst,
#       BlendSrcOver    = agg::comp_op_src_over,
#       BlendDstOver    = agg::comp_op_dst_over,
#       BlendSrcIn      = agg::comp_op_src_in,
#       BlendDstIn      = agg::comp_op_dst_in,
#       BlendSrcOut     = agg::comp_op_src_out,
#       BlendDstOut     = agg::comp_op_dst_out,
#       BlendSrcAtop    = agg::comp_op_src_atop,
#       BlendDstAtop    = agg::comp_op_dst_atop,
#       BlendXor        = agg::comp_op_xor,
#       BlendAdd        = agg::comp_op_plus,
#       BlendMultiply   = agg::comp_op_multiply,
#       BlendScreen     = agg::comp_op_screen,
#       BlendOverlay    = agg::comp_op_overlay,
#       BlendDarken     = agg::comp_op_darken,
#       BlendLighten    = agg::comp_op_lighten,
#       BlendColorDodge = agg::comp_op_color_dodge,
#       BlendColorBurn  = agg::comp_op_color_burn,
#       BlendHardLight  = agg::comp_op_hard_light,
#       BlendSoftLight  = agg::comp_op_soft_light,
#       BlendDifference = agg::comp_op_difference,
#       BlendExclusion  = agg::comp_op_exclusion,
#   };
#
#   enum Direction
#   {
#       CW, CCW
#   };
