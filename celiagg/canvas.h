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

#pragma once

#define _USE_MATH_DEFINES
#include <cmath>

#include <agg_alpha_mask_u8.h>
#include <agg_bezier_arc.h>
#include <agg_bspline.h>
#include <agg_conv_bspline.h>
#include <agg_conv_contour.h>
#include <agg_conv_curve.h>
#include <agg_conv_dash.h>
#include <agg_conv_stroke.h>
#include <agg_path_storage.h>
#include <agg_pixfmt_amask_adaptor.h>
#include <agg_pixfmt_gray.h>
#include <agg_pixfmt_rgb.h>
#include <agg_pixfmt_rgba.h>
#include <agg_rasterizer_scanline_aa.h>
#include <agg_renderer_base.h>
#include <agg_renderer_scanline.h>
#include <agg_rendering_buffer.h>
#include <agg_scanline_p.h>

#include "canvas_impl.h"
#include "font_cache.h"
#include "glyph_iter.h"
#include "graphics_state.h"
#include "image.h"
#include "paint.h"
#include "vertex_source.h"


template<typename pixfmt_t>
class canvas : public canvas_base
{
public:
    canvas(unsigned char* buf,
           const unsigned width, const unsigned height, const int stride,
           const size_t channel_count, FontCache& cache,
           const bool bottom_up = false);
    virtual ~canvas() {}

    const size_t channel_count() const;
    unsigned width() const;
    unsigned height() const;

    void clear(const double r, const double g, const double b, const double a = 1.0);

    void draw_image(Image& img,
                    const agg::trans_affine& transform,
                    const GraphicsState& gs);
    void draw_shape(VertexSource& shape,
                    const agg::trans_affine& transform,
                    Paint& linePaint, Paint& fillPaint,
                    const GraphicsState& gs);
    void draw_shape_at_points(VertexSource& shape,
                              const double* points,
                              const size_t point_count,
                              const agg::trans_affine& transform,
                              Paint& linePaint, Paint& fillPaint,
                              const GraphicsState& gs);
    void draw_text(const char* text, Font& font,
                   const agg::trans_affine& transform,
                   Paint& linePaint, Paint& fillPaint,
                   const GraphicsState& gs);

protected:

    typedef agg::amask_no_clip_gray8 alpha_mask_t;
    typedef agg::pixfmt_amask_adaptor<pixfmt_t, alpha_mask_t> masked_pxfmt_t;
    typedef agg::renderer_base<masked_pxfmt_t> masked_renderer_t;

    typedef agg::renderer_base<pixfmt_t> renderer_t;
    typedef agg::rasterizer_scanline_aa<> rasterizer_t;

    size_t m_channel_count;
    FontCache& m_font_cache;
    agg::rendering_buffer m_renbuf;
    pixfmt_t m_pixfmt;
    renderer_t m_renderer;
    rasterizer_t m_rasterizer;
    agg::scanline_p8 m_scanline;
    bool m_bottom_up;

private:

    template<typename base_renderer_t>
    void _draw_image_internal(Image& img,
                              const agg::trans_affine& transform,
                              const GraphicsState& gs,
                              base_renderer_t& renderer);
    template<typename base_renderer_t, typename span_gen_t>
    void _draw_image_final(base_renderer_t& renderer,
                           span_gen_t& span_generator);
    template<typename base_renderer_t>
    void _draw_shape_internal(VertexSource& shape,
                              const agg::trans_affine& transform,
                              Paint& linePaint, Paint& fillPaint,
                              const GraphicsState& gs,
                              base_renderer_t& renderer);
    template<typename base_renderer_t>
    void _draw_shape_stroke_setup(VertexSource& shape,
                                  const agg::trans_affine& mtx,
                                  Paint& paint,
                                  const GraphicsState& gs,
                                  base_renderer_t& renderer);
    template<typename stroke_t, typename base_renderer_t>
    void _draw_shape_stroke_final(stroke_t& stroke,
                                  const agg::trans_affine& mtx,
                                  Paint& paint,
                                  const GraphicsState& gs,
                                  base_renderer_t& renderer);
    template<typename base_renderer_t>
    void _draw_text_internal(const char* text, Font& font,
                             const agg::trans_affine& transform,
                             Paint& linePaint, Paint& fillPaint,
                             const GraphicsState& gs,
                             base_renderer_t& renderer);
    template<typename base_renderer_t>
    void _draw_text_raster(GlyphIterator& iterator,
                           Font& font,
                           const agg::trans_affine& transform,
                           Paint& fillPaint,
                           const GraphicsState& gs,
                           base_renderer_t& renderer);
    template<typename base_renderer_t>
    void _draw_text_vector(GlyphIterator& iterator,
                           Font& font,
                           const agg::trans_affine& transform,
                           Paint& linePaint, Paint& fillPaint,
                           const GraphicsState& gs,
                           base_renderer_t& renderer);

    GraphicsState::DrawingMode _convert_text_mode(const GraphicsState::TextDrawingMode tm);
    inline void _set_aa(const bool& aa);
    inline void _set_clipping(const GraphicsState::Rect& rect);

private:
    // Target buffer/numpy array must be supplied to constructor.  The following line ensures that no default 
    // constructor is user-accessible, even if the buffer-accepting constructor is deleted (presence of a constructor 
    // taking one or more parameters suppresses automatic default constructor generation).
    canvas(){}
    // canvases are non-copyable in order to dodge the question of whether a copied canvas refers to the
    // original's buffer/numpy array or receives its own copy.  The compiler does autogenerate assignment and copy 
    // constructors if these are not defined; so, we define them such that they are not user-accessible.
    canvas(const canvas&){}
    canvas& operator = (const canvas&){ return *this; }
};

// The implementation is broken out into several files to keep things focused
#include "canvas.hxx"
#include "canvas_image.hxx"
#include "canvas_shape.hxx"
#include "canvas_text.hxx"
