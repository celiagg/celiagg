// The MIT License (MIT)
// 
// Copyright (c) 2014-2015 WUSTL ZPLAB
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

#pragma once

#define _USE_MATH_DEFINES
#include <cmath>

// for integer typedefs
#include <stdint.h>

#include <agg_bezier_arc.h>
#include <agg_bspline.h>
#include <agg_conv_bspline.h>
#include <agg_conv_contour.h>
#include <agg_conv_curve.h>
#include <agg_conv_stroke.h>
#include <agg_path_storage.h>
#include <agg_pixfmt_gray.h>
#include <agg_pixfmt_rgb.h>
#include <agg_pixfmt_rgba.h>
#include <agg_rasterizer_scanline_aa.h>
#include <agg_renderer_base.h>
#include <agg_renderer_scanline.h>
#include <agg_rendering_buffer.h>
#include <agg_scanline_p.h>
#include <ctrl/agg_polygon_ctrl.h>

#include "glyph_iter.h"
#include "graphics_state.h"
#include "image.h"
#include "paint.h"
#include "text.h"

// Interface to ndarray_canvas that is generic for all pixfmts, for the
// convenience of being able to implement functionality common to cython
// wrappers of ndarray_canvas template instances representing pixfmts
// in a cython base class.
class ndarray_canvas_base
{
public:
    virtual ~ndarray_canvas_base(){}

    virtual const size_t channel_count() const = 0;
    virtual unsigned width() const = 0;
    virtual unsigned height() const = 0;

    virtual void draw_path(const agg::path_storage& path,
                           const agg::trans_affine& transform,
                           Paint& linePaint, Paint& fillPaint,
                           const GraphicsState& gs) = 0;
    virtual void draw_bspline(const double* points, const size_t& point_count,
                              const agg::trans_affine& transform,
                              Paint& linePaint, Paint& fillPaint,
                              const GraphicsState& gs) = 0;
    virtual void draw_image(Image& img,
                            const agg::trans_affine& transform,
                            const GraphicsState& gs) = 0;
    virtual void draw_text(const char* text, Font& font,
                           const agg::trans_affine& transform,
                           Paint& linePaint, Paint& fillPaint,
                           const GraphicsState& gs) = 0;
};

template<typename pixfmt_t>
class ndarray_canvas : public ndarray_canvas_base
{
public:
    ndarray_canvas(uint8_t* buf,
                   const unsigned width, const unsigned height, const int stride,
                   const size_t channel_count);
    virtual ~ndarray_canvas(){}

    const size_t channel_count() const;
    unsigned width() const;
    unsigned height() const;

    void draw_path(const agg::path_storage& path,
                   const agg::trans_affine& transform,
                   Paint& linePaint, Paint& fillPaint,
                   const GraphicsState& gs);
    void draw_bspline(const double* points, const size_t& point_count,
                      const agg::trans_affine& transform,
                      Paint& linePaint, Paint& fillPaint,
                      const GraphicsState& gs);
    void draw_image(Image& img,
                    const agg::trans_affine& transform,
                    const GraphicsState& gs);
    void draw_text(const char* text, Font& font,
                   const agg::trans_affine& transform,
                   Paint& linePaint, Paint& fillPaint,
                   const GraphicsState& gs);
protected:

    typedef agg::renderer_base<pixfmt_t> renderer_t;
    typedef agg::rasterizer_scanline_aa<> rasterizer_t;

    size_t m_channel_count;
    agg::rendering_buffer m_renbuf;
    pixfmt_t m_pixfmt;
    renderer_t m_renderer;
    rasterizer_t m_rasterizer;
    agg::scanline_p8 m_scanline;

    inline void set_aa(const bool& aa);
    void _draw_text_raster(GlyphIterator& iterator, Font& font, Paint& linePaint,
                           Paint& fillPaint, const GraphicsState& gs);
    void _draw_text_vector(GlyphIterator& iterator, Font& font, agg::trans_affine& transform,
                           Paint& linePaint, Paint& fillPaint, const GraphicsState& gs);

private:
    // Target buffer/numpy array must be supplied to constructor.  The following line ensures that no default 
    // constructor is user-accessible, even if the buffer-accepting constructor is deleted (presence of a constructor 
    // taking one or more parameters suppresses automatic default constructor generation). 
    ndarray_canvas(){}
    // ndarray_canvases are non-copyable in order to dodge the question of whether a copied ndarray_canvas refers to the 
    // original's buffer/numpy array or receives its own copy.  The compiler does autogenerate assignment and copy 
    // constructors if these are not defined; so, we define them such that they are not user-accessible. 
    ndarray_canvas(const ndarray_canvas&){}
    ndarray_canvas& operator = (const ndarray_canvas&){ return *this; }
};

#include "ndarray_canvas.hxx"
