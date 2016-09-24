// The MIT License (MIT)
//
// Copyright (c) 2016 WUSTL ZPLAB
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// Authors: John Wiggins

#ifndef PYAGG_PAINT_H
#define PYAGG_PAINT_H

#include <stdint.h>

#include <agg_array.h>
#include <agg_basics.h>
#include <agg_color_rgba.h>
#include <agg_pixfmt_rgb.h>
#include <agg_renderer_scanline.h>
#include <agg_scanline_u.h>
#include <agg_span_allocator.h>
#include <agg_span_gradient.h>
#include <agg_span_interpolator_linear.h>
#include <agg_trans_affine.h>

#include "image.h"

struct GradientStop
{
    // NOTE: This struct should remain simple so that a double* can be trivially
    // converted to an array of GradientStop with reinterpret_cast<>.
    double off;
    double r;
    double g;
    double b;
    double a;

    GradientStop(const double o, const double _r, const double _g,
                 const double _b, const double _a) :
                 off(o), r(_r), g(_g), b(_b), a(_a) {}
};


class Paint
{
public:
    enum PaintType
    {
        k_PaintTypeSolid = 0,
        k_PaintTypeLinearGradient,
        k_PaintTypeRadialGradient,
        k_PaintTypePattern
    };

    enum GradientSpread
    {
        k_GradientSpreadInvalid = -1,
        k_GradientSpreadPad = 0,
        k_GradientSpreadReflect,
        k_GradientSpreadRepeat
    };

    enum GradientUnits
    {
        k_GradientUnitsInvalid = -1,
        k_GradientUnitsUserSpace = 0,
        k_GradientUnitsObjectBoundingBox
    };

    enum PatternStyle
    {
        k_PatternStyleInvalid = -1,
        k_PatternStyleRepeat = 0,
        k_PatternStyleReflect
    };

private:
    enum LinearPoints
    {
        k_LinearX1 = 0,
        k_LinearY1,
        k_LinearX2,
        k_LinearY2,
        k_LinearPointsSize
    };

    enum RadialPoints
    {
        k_RadialCX = 0,
        k_RadialCY,
        k_RadialR,
        k_RadialFX,
        k_RadialFY,
        k_RadialPointsSize
    };

public:

    Paint(const double r, const double g, const double b, const double a);
    Paint(const PaintType type, double* points, const unsigned n_points,
          double* stops, const unsigned n_stops,
          const GradientSpread spread = k_GradientSpreadPad,
          const GradientUnits units = k_GradientUnitsUserSpace);
    Paint(const PatternStyle style, Image* img);

    double  a() const;
    void  a(const double);

    double  r() const;
    void  r(const double);

    double  g() const;
    void  g(const double);

    double  b() const;
    void  b(const double);

    void transform(const agg::trans_affine& mat);
    const agg::trans_affine& transform() const;

    template <typename pixfmt_t, typename rasterizer_t, typename renderer_t>
    void render(rasterizer_t& ras, renderer_t& renderer);

private:

    template <typename pixfmt_t, typename rasterizer_t, typename renderer_t>
    void _render_linear_grad(rasterizer_t& ras, renderer_t& renderer);

    template <typename pixfmt_t, typename rasterizer_t, typename renderer_t>
    void _render_radial_grad(rasterizer_t& ras, renderer_t& renderer);

    template <typename pixfmt_t, typename rasterizer_t, typename renderer_t, typename grad_func_t, typename vector_t>
    void _render_spread_grad(rasterizer_t& ras, renderer_t& renderer, grad_func_t& func, vector_t& points);

    template <typename pixfmt_t, typename rasterizer_t, typename renderer_t, typename grad_func_t, typename vector_t>
    void _render_gradient_final(rasterizer_t& ras, renderer_t& renderer, grad_func_t& func, vector_t& points);

    template <typename pixfmt_t, typename rasterizer_t, typename renderer_t>
    void _render_pattern(rasterizer_t& ras, renderer_t& renderer);

    template <typename pixfmt_t, typename rasterizer_t, typename renderer_t, typename source_t, typename span_gen_t>
    void _render_pattern_final(rasterizer_t& ras, renderer_t& renderer);

    template <typename pixfmt_t, typename rasterizer_t, typename renderer_t>
    void _render_solid(rasterizer_t& ras, renderer_t& renderer);

private:
    typedef agg::pod_array_adaptor<double> PointArray;
    typedef agg::pod_array_adaptor<GradientStop> StopArray;
 
    PointArray          m_points;
    StopArray           m_stops;
    agg::trans_affine   m_transform;

    agg::rgba           m_color;

    Image*              m_image;

    PaintType           m_type;
    GradientSpread      m_spread;
    GradientUnits       m_units;
    PatternStyle        m_pattern_style;
};

#include "paint.hxx"

#endif // PYAGG_PAINT_H
