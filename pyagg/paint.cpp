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

#include "paint.h"

Paint::Paint(const double r, const double g, const double b, const double a)
: m_points(NULL, 0)
, m_stops(NULL, 0)
, m_color(r, g, b, a)
, m_image(NULL)
, m_type(k_PaintTypeSolid)
, m_spread(k_GradientSpreadInvalid)
, m_units(k_GradientUnitsInvalid)
, m_pattern_style(k_PatternStyleInvalid)
{
}

Paint::Paint(const PaintType type,
             double* points, const unsigned n_points,
             double* stops, const unsigned n_stops,
             const GradientSpread spread, const GradientUnits units)
: m_points(points, n_points)
, m_stops(reinterpret_cast<GradientStop*>(stops), n_stops)
, m_color(0.0, 0.0, 0.0, 0.0)
, m_image(NULL)
, m_type(type)
, m_spread(spread)
, m_units(units)
, m_pattern_style(k_PatternStyleInvalid)
{
}

Paint::Paint(const PatternStyle style, Image* img)
: m_points(NULL, 0)
, m_stops(NULL, 0)
, m_color(0.0, 0.0, 0.0, 0.0)
, m_image(img)
, m_type(k_PaintTypePattern)
, m_spread(k_GradientSpreadInvalid)
, m_units(k_GradientUnitsUserSpace)
, m_pattern_style(style)
{
}

void Paint::transform(const agg::trans_affine& mat)
{
    m_transform = mat;
}

const agg::trans_affine& Paint::transform() const
{
    return m_transform;
}

double  Paint::a() const
{
    return m_color.a;
}

void  Paint::a(const double _a)
{
    m_color.a = _a;
}

double  Paint::r() const
{
    return m_color.r;
}

void  Paint::r(const double _r)
{
    m_color.r = _r;
}

double  Paint::g() const
{
    return m_color.g;
}

void  Paint::g(const double _g)
{
    m_color.g = _g;
}

double  Paint::b() const
{
    return m_color.b;
}

void  Paint::b(const double _b)
{
    m_color.b = _b;
}
