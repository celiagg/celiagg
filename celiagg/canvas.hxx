// The MIT License (MIT)
//
// Copyright (c) 2014-2015 WUSTL ZPLAB
// Copyright (c) 2016-2023 Celiagg Contributors
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

// NOTE: The renderer object constructed here keeps pointers to objects which
// are allocated on the stack, so in order to avoid code duplication, we get
// this funky macro...
#define _WITH_MASKED_RENDERER(gs, name) \
Image* stencil = const_cast<Image*>(gs.stencil());\
agg::rendering_buffer& stencilbuf = stencil->get_buffer();\
alpha_mask_t stencil_mask(stencilbuf);\
masked_pxfmt_t masked_pixfmt(m_pixfmt, stencil_mask);\
masked_renderer_t name(masked_pixfmt);


template<typename pixfmt_t>
canvas<pixfmt_t>::canvas(unsigned char* buf,
    const unsigned width, const unsigned height, const int stride,
    const size_t channel_count, FontCache& cache, const bool bottom_up)
: m_channel_count(channel_count)
, m_font_cache(cache)
, m_renbuf(buf, width, height, bottom_up ? -stride : stride)
, m_pixfmt(m_renbuf)
, m_renderer(m_pixfmt)
, m_bottom_up(bottom_up)
{
}

template<typename pixfmt_t>
const size_t canvas<pixfmt_t>::channel_count() const
{
//  typedef typename pixfmt_t::order_type T;
//  return T::N;
    // Something like the above should work, but the cop-out of just storing channel count
    // ourselves definitely works.
    return m_channel_count;
}

template<typename pixfmt_t>
unsigned canvas<pixfmt_t>::width() const
{
    return m_renbuf.width();
}

template<typename pixfmt_t>
unsigned canvas<pixfmt_t>::height() const
{
    return m_renbuf.height();
}

template<typename pixfmt_t>
void canvas<pixfmt_t>::clear(const double r, const double g,
    const double b, const double a)
{
    typename pixfmt_t::color_type c(agg::rgba(r, g, b, a));
    m_renderer.clear(c);
}

template<typename pixfmt_t>
void canvas<pixfmt_t>::draw_image(Image& img,
    const agg::trans_affine& transform, const GraphicsState& gs)
{
    _set_clipping(gs.clip_box());
    if (gs.stencil() == NULL)
    {
        _draw_image_internal<renderer_t>(img, transform, gs, m_renderer);
    }
    else
    {
        _WITH_MASKED_RENDERER(gs, renderer)
        _draw_image_internal<masked_renderer_t>(img, transform, gs, renderer);
    }
}

template<typename pixfmt_t>
void canvas<pixfmt_t>::draw_shape(VertexSource& shape,
    const agg::trans_affine& transform, Paint& linePaint, Paint& fillPaint,
    const GraphicsState& gs)
{
    _set_clipping(gs.clip_box());
    linePaint.master_alpha(gs.master_alpha());
    fillPaint.master_alpha(gs.master_alpha());

    if (gs.stencil() == NULL)
    {
        _draw_shape_internal(shape, transform, linePaint, fillPaint, gs, m_renderer);
    }
    else
    {
        _WITH_MASKED_RENDERER(gs, renderer)
        _draw_shape_internal(shape, transform, linePaint, fillPaint, gs, renderer);
    }
}

template<typename pixfmt_t>
void canvas<pixfmt_t>::draw_shape_at_points(VertexSource& shape,
    const double* points, const size_t point_count,
    const agg::trans_affine& transform, Paint& linePaint, Paint& fillPaint,
    const GraphicsState& gs)
{
    agg::simple_polygon_vertex_source _points(points, point_count, false, false);
    agg::trans_affine pt_trans;
    unsigned cmd;

    _set_clipping(gs.clip_box());
    linePaint.master_alpha(gs.master_alpha());
    fillPaint.master_alpha(gs.master_alpha());

    if (gs.stencil() == NULL)
    {
        for (;;)
        {
            cmd = _points.vertex(&pt_trans.tx, &pt_trans.ty);
            if (cmd == agg::path_cmd_end_poly) break;

            _draw_shape_internal(shape, pt_trans * transform, linePaint, fillPaint, gs, m_renderer);
        }
    }
    else
    {
        _WITH_MASKED_RENDERER(gs, renderer)
        for (;;)
        {
            cmd = _points.vertex(&pt_trans.tx, &pt_trans.ty);
            if (cmd == agg::path_cmd_end_poly) break;

            _draw_shape_internal(shape, pt_trans * transform, linePaint, fillPaint, gs, renderer);
        }
    }
}

template<typename pixfmt_t>
void canvas<pixfmt_t>::draw_text(const char* text,
    Font& font, const agg::trans_affine& transform,
    Paint& linePaint, Paint& fillPaint, const GraphicsState& gs,
    double* cursorX, double* cursorY)
{
    _set_clipping(gs.clip_box());
    linePaint.master_alpha(gs.master_alpha());
    fillPaint.master_alpha(gs.master_alpha());

    if (gs.stencil() == NULL)
    {
        _draw_text_internal(text, font, transform, linePaint, fillPaint, gs, m_renderer, cursorX, cursorY);
    }
    else
    {
        _WITH_MASKED_RENDERER(gs, renderer)
        _draw_text_internal(text, font, transform, linePaint, fillPaint, gs, renderer, cursorX, cursorY);
    }
}

template<typename pixfmt_t>
void canvas<pixfmt_t>::_set_aa(const bool& aa)
{
    if(aa)
    {
        m_rasterizer.gamma(agg::gamma_linear());
    }
    else
    {
        m_rasterizer.gamma(agg::gamma_threshold(0.5));
    }
}

template<typename pixfmt_t>
void canvas<pixfmt_t>::_set_clipping(const GraphicsState::Rect& rect)
{
    if (rect.is_valid())
    {
        m_rasterizer.clip_box(rect.x1, rect.y1, rect.x2, rect.y2);
    }
    else
    {
        m_rasterizer.reset_clipping();
    }
}
