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

#cdef extern from "agg_trans_affine.h" namespace "agg":
#    cdef cppclass trans_affine:
#        double sx, shy, shx, sy, tx, ty
#
#        trans_affine()
#        trans_affine(double v0, double v1, double v2,
#                     double v3, double v4, double v5)
#        explicit trans_affine(const double* m)
#
#        trans_affine(double x1, double y1, double x2, double y2,
#                     const double* parl)
#
#        trans_affine(const double* parl,
#                     double x1, double y1, double x2, double y2)
#
#        trans_affine(const double* src, const double* dst)
#
#        const trans_affine& parl_to_parl(const double* src,
#                                         const double* dst)
#        const trans_affine& rect_to_parl(double x1, double y1,
#                                         double x2, double y2,
#                                         const double* parl)
#        const trans_affine& parl_to_rect(const double* parl,
#                                         double x1, double y1,
#                                         double x2, double y2)
#
#        const trans_affine& reset()
#
#        const trans_affine& translate(double x, double y)
#        const trans_affine& rotate(double a)
#        const trans_affine& scale(double s)
#        const trans_affine& scale(double x, double y)
#
#        const trans_affine& multiply(const trans_affine& m)
#        const trans_affine& premultiply(const trans_affine& m)
#        const trans_affine& multiply_inv(const trans_affine& m)
#        const trans_affine& premultiply_inv(const trans_affine& m)
#        const trans_affine& invert()
#        const trans_affine& flip_x();
#        const trans_affine& flip_y();
#
#        void store_to(double* m) const
#        const trans_affine& load_from(const double* m)
#
#        const trans_affine& operator *= (const trans_affine& m)
#        const trans_affine& operator /= (const trans_affine& m)
#        trans_affine operator * (const trans_affine& m) const
#        trans_affine operator / (const trans_affine& m) const
#        trans_affine operator ~ () const
#        bool operator == (const trans_affine& m) const
#        bool operator != (const trans_affine& m) const
#
#        void transform(double* x, double* y) const
#        void transform_2x2(double* x, double* y) const
#        void inverse_transform(double* x, double* y) const
#
#        double determinant() const
#        double determinant_reciprocal() const
#
#        double scale() const
#
#        bool is_valid(double epsilon = affine_epsilon) const
#        bool is_identity(double epsilon = affine_epsilon) const
#        bool is_equal(const trans_affine& m, double epsilon = affine_epsilon) const
#
#        double rotation() const
#        void   translation(double* dx, double* dy) const
#        void   scaling(double* x, double* y) const
#        void   scaling_abs(double* x, double* y) const

cdef extern from "agg2d/agg2d.h" namespace "Agg2D":
    cdef cppclass Image:
        Image()
        Image(unsigned char* buf, unsigned width, unsigned height, int stride)
        void attach(unsigned char* buf, unsigned width, unsigned height, int stride)
        int width() const
        int height() const
        void premultiply()
        void demultiply()
    cdef cppclass RectD:
        double x1
        double y1
        double x2
        double y2
        RectD()
        RectD(double x1, double y1, double x2, double y2)
    cdef enum LineJoin:
        JoinMiter
        JoinRound
        JoinBevel
    cdef enum LineCap:
        CapButt
        CapSquare
        CapRound
    cdef enum TextAlignment:
        AlignLeft
        AlignRight
        AlignCenter
        AlignTop
        AlignBottom
    cdef enum DrawPathFlag:
        FillOnly
        StrokeOnly
        FillAndStroke
        FillWithLineColor
    cdef enum ViewportOption:
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
    cdef enum ImageFilter:
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
    cdef enum ImageResample:
        NoResample
        ResampleAlways
        ResampleOnZoomOut
    cdef enum FontCacheType:
        RasterFontCache
        VectorFontCache
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
    cdef enum Direction:
        CW
        CCW

cdef extern from "agg2d/agg2d.h":
    cdef cppclass Agg2D:
        Agg2D()
        void attach(unsigned char* buf, unsigned width, unsigned height, int stride)
        void attach(Image& img)

        void  clipBox(double x1, double y1, double x2, double y2)
        RectD clipBox() const

        void blendMode(BlendMode m)
        BlendMode blendMode() const

        void imageBlendMode(BlendMode m)
        BlendMode imageBlendMode() const

        void imageBlendColor(unsigned r, unsigned g, unsigned b, unsigned a)
        srgba8 imageBlendColor() const

        void masterAlpha(double a)
        double masterAlpha() const

        void antiAliasGamma(double g)
        double antiAliasGamma() const

        void fillColor(unsigned r, unsigned g, unsigned b, unsigned a)
        void noFill()

        void lineColor(unsigned r, unsigned g, unsigned b, unsigned a)
        void noLine()

        srgba8 fillColor() const
        srgba8 lineColor() const

        void lineWidth(double w)
        double lineWidth() const

        void lineCap(LineCap cap)
        LineCap lineCap() const

        void lineJoin(LineJoin join)
        LineJoin lineJoin() const

        void fillEvenOdd(bool evenOddFlag)
        bool fillEvenOdd() const

#       Transformations transformations() const
#       void transformations(const double* tr)
#       void resetTransformations()
#       void affine(const double* tr)
#       void rotate(double angle)
#       void scale(double sx, double sy)
#       void skew(double sx, double sy)
#       void translate(double x, double y)
#       void parallelogram(double x1, double y1, double x2, double y2, const double* para)
#       void viewport(double worldX1,  double worldY1,  double worldX2,  double worldY2,
#                     double screenX1, double screenY1, double screenX2, double screenY2,
#                     ViewportOption opt)

        void line(double x1, double y1, double x2, double y2)
        void triangle(double x1, double y1, double x2, double y2, double x3, double y3)
        void rectangle(double x1, double y1, double x2, double y2)
        void roundedRect(double x1, double y1, double x2, double y2, double r)
        void roundedRect(double x1, double y1, double x2, double y2, double rx, double ry)
        void roundedRect(double x1, double y1, double x2, double y2,
                         double rxBottom, double ryBottom,
                         double rxTop,    double ryTop)
        void ellipse(double cx, double cy, double rx, double ry)
        void arc(double cx, double cy, double rx, double ry, double start, double sweep)
        void star(double cx, double cy, double r1, double r2, double startAngle, int numRays)
        void curve(double x1, double y1, double x2, double y2, double x3, double y3)
        void curve(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4)
        void polygon(double* xy, int numPoints)
        void polyline(double* xy, int numPoints)

        void resetPath()
        void addEllipse(double cx, double cy, double rx, double ry, Direction dir)
        void closePolygon()

        void flipText(bool flip)
        bool flipText() const
        void font(const char* fileName, double height, bool bold, bool italic, FontCacheType ch, double angle);
        double fontHeight() const
        void textHAlignment(TextAlignment alignX)
        TextAlignment textHAlignment() const
        void textVAlignment(TextAlignment alignY)
        TextAlignment textVAlignment() const
        bool textHints() const
        void textHints(bool hints)
        double textWidth(const char* str)
        void text(double x, double y, const char* str, bool roundOff, double dx, double dy)

        void drawPath(DrawPathFlag flag)
#       void drawPathNoTransform(DrawPathFlag flag)

        void imageFilter(ImageFilter f)
        ImageFilter imageFilter() const

        void imageResample(ImageResample f)
        ImageResample imageResample() const

        void transformImage(const Image& img,
                            int img_x0, int img_y0, int img_x1, int img_y1,
                            double dst_x0, double dst_y0, double dst_x1, double dst_y1)
        void transformImage(const Image& img,
                            double dst_x0, double dst_y0, double dst_x1, double dst_y1)
        void transformImage(const Image& img,
                            int img_x0, int img_y0, int img_x1, int img_y1,
                            const double* parallelogram)
        void transformImage(const Image& img, const double* parallelogram)
        void transformImagePath(const Image& img,
                                int img_x0, int img_y0, int img_x1, int img_y1,
                                double dst_x0, double dst_y0, double dst_x1, double dst_y1)
        void transformImagePath(const Image& img,
                                double dst_x0, double dst_y0, double dst_x1, double dst_y1)
        void transformImagePath(const Image& img,
                                int img_x0, int img_y0, int img_x1, int img_y1,
                                const double* parallelogram)
        void transformImagePath(const Image& img, const double* parallelogram)
        void blendImage(Image& img,
                        int img_x0, int img_y0, int img_x1, int img_y1,
                        double dst_x, double dst_y, unsigned alpha)
        void blendImage(Image& img, double dst_x, double dst_y, unsigned alpha)
        void copyImage(Image& img,
                       int img_x0, int img_y0, int img_x1, int img_y1,
                       double dst_x, double dst_y)
        void copyImage(Image& img, double dst_x, double dst_y)


