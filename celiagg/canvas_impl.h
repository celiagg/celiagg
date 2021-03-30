// The MIT License (MIT)
//
// Copyright (c) 2014-2015 WUSTL ZPLAB
// Copyright (c) 2016-2021 Celiagg Contributors
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
// Authors: Erik Hvatum <ice.rikh@gmail.com>
//          John Wiggins

#ifndef CELIAGG_CANVAS_IMPL_H
#define CELIAGG_CANVAS_IMPL_H

#include <agg_trans_affine.h>

#include "font_cache.h"
#include "graphics_state.h"
#include "image.h"
#include "paint.h"
#include "vertex_source.h"

class canvas_base;

// Factory functions for each of the supported template specializations of canvas
canvas_base* canvas_rgba128(unsigned char* buf,
    const unsigned width, const unsigned height, const int stride,
    FontCache& cache, const bool bottom_up = false);
canvas_base* canvas_bgra32(unsigned char* buf,
    const unsigned width, const unsigned height, const int stride,
    FontCache& cache, const bool bottom_up = false);
canvas_base* canvas_rgba32(unsigned char* buf,
    const unsigned width, const unsigned height, const int stride,
    FontCache& cache, const bool bottom_up = false);
canvas_base* canvas_rgb24(unsigned char* buf,
    const unsigned width, const unsigned height, const int stride,
    FontCache& cache, const bool bottom_up = false);
canvas_base* canvas_ga16(unsigned char* buf,
    const unsigned width, const unsigned height, const int stride,
    FontCache& cache, const bool bottom_up = false);
canvas_base* canvas_g8(unsigned char* buf,
    const unsigned width, const unsigned height, const int stride,
    FontCache& cache, const bool bottom_up = false);

// Interface to canvas that is generic for all pixfmts, for the
// convenience of being able to implement functionality common to cython
// wrappers of canvas template instances representing pixfmts
// in a cython base class.
class canvas_base
{
public:
    virtual ~canvas_base(){}

    virtual const size_t channel_count() const = 0;
    virtual unsigned width() const = 0;
    virtual unsigned height() const = 0;

    virtual void clear(const double r, const double g,
                       const double b, const double a) = 0;

    virtual void draw_image(Image& img,
                            const agg::trans_affine& transform,
                            const GraphicsState& gs) = 0;
    virtual void draw_shape(VertexSource& shape,
                            const agg::trans_affine& transform,
                            Paint& linePaint, Paint& fillPaint,
                            const GraphicsState& gs) = 0;
    virtual void draw_shape_at_points(VertexSource& shape,
                                      const double* points,
                                      const size_t point_count,
                                      const agg::trans_affine& transform,
                                      Paint& linePaint, Paint& fillPaint,
                                      const GraphicsState& gs) = 0;
    virtual void draw_text(const char* text, Font& font,
                           const agg::trans_affine& transform,
                           Paint& linePaint, Paint& fillPaint,
                           const GraphicsState& gs,
                           double* cursorX, double* cursorY) = 0;
};

#endif // CELIAGG_CANVAS_IMPL_H
