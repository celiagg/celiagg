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
ndarray_canvas<pixfmt_t>::ndarray_canvas(unsigned char* buf,
    const unsigned width, const unsigned height, const int stride,
    const size_t channel_count, const bool bottom_up)
: m_channel_count(channel_count)
, m_renbuf(buf, width, height, bottom_up ? -stride : stride)
, m_pixfmt(m_renbuf)
, m_renderer(m_pixfmt)
, m_bottom_up(bottom_up)
{
}

template<typename pixfmt_t>
const size_t ndarray_canvas<pixfmt_t>::channel_count() const
{
//  typedef typename pixfmt_t::order_type T;
//  return T::N;
    // Something like the above should work, but the cop-out of just storing channel count 
    // ourselves definitely works. 
    return m_channel_count;
}

template<typename pixfmt_t>
unsigned ndarray_canvas<pixfmt_t>::width() const
{
    return m_renbuf.width();
}

template<typename pixfmt_t>
unsigned ndarray_canvas<pixfmt_t>::height() const
{
    return m_renbuf.height();
}

template<typename pixfmt_t>
void ndarray_canvas<pixfmt_t>::clear(const double r, const double g,
    const double b, const double a)
{
    typename pixfmt_t::color_type c(agg::rgba(r, g, b, a));
    m_renderer.clear(c);
}

template<typename pixfmt_t>
void ndarray_canvas<pixfmt_t>::draw_image(Image& img,
    const agg::trans_affine& transform, const GraphicsState& gs)
{
    typedef typename image_filters<pixfmt_t>::nearest_t span_gen_t;

    _set_clipping(gs.clip_box());
    // XXX: Apply master alpha here somehow!

    if (gs.stencil() == NULL)
    {
        _draw_image_internal<renderer_t, span_gen_t>(img, transform, gs, m_renderer);
    }
    else
    {
        _WITH_MASKED_RENDERER(gs, renderer)
        _draw_image_internal<masked_renderer_t, span_gen_t>(img, transform, gs, renderer);
    }
}

template<typename pixfmt_t>
void ndarray_canvas<pixfmt_t>::draw_shape(VertexSource& shape,
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
void ndarray_canvas<pixfmt_t>::draw_text(const char* text,
    Font& font, const agg::trans_affine& transform,
    Paint& linePaint, Paint& fillPaint, const GraphicsState& gs)
{
    _set_clipping(gs.clip_box());
    linePaint.master_alpha(gs.master_alpha());
    fillPaint.master_alpha(gs.master_alpha());

    // Make sure the font is activated
    font.activate();

    if (gs.stencil() == NULL)
    {
        _draw_text_internal(text, font, transform, linePaint, fillPaint, gs, m_renderer);
    }
    else
    {
        _WITH_MASKED_RENDERER(gs, renderer)
        _draw_text_internal(text, font, transform, linePaint, fillPaint, gs, renderer);
    }
}

template<typename pixfmt_t>
template<typename base_renderer_t, typename span_gen_t>
void ndarray_canvas<pixfmt_t>::_draw_image_internal(Image& img,
    const agg::trans_affine& transform, const GraphicsState& gs,
    base_renderer_t& renderer)
{
    typedef typename image_filters<pixfmt_t>::source_t source_t;
    typedef typename agg::span_allocator<typename pixfmt_t::color_type> span_alloc_t;
    typedef agg::renderer_scanline_aa<base_renderer_t, span_alloc_t, span_gen_t> img_renderer_t;
    typedef agg::conv_transform<agg::path_storage> trans_curve_t;

    agg::path_storage img_outline = img.image_outline();
    pixfmt_t src_pix(img.get_buffer());

    agg::trans_affine src_mtx = transform;
    agg::trans_affine inv_img_mtx = transform;
    inv_img_mtx.invert();
    interpolator_t interpolator(inv_img_mtx);

    typename pixfmt_t::color_type back_color(agg::rgba(0.5, 0.5, 0.5, 1.0));
    source_t source(src_pix, back_color);
    span_gen_t span_generator(source, interpolator);
    span_alloc_t span_allocator;
    img_renderer_t img_renderer(renderer, span_allocator, span_generator);

    m_rasterizer.reset();
    trans_curve_t trans_outline(img_outline, src_mtx);
    m_rasterizer.add_path(trans_outline);
    agg::render_scanlines(m_rasterizer, m_scanline, img_renderer);
}

template<typename pixfmt_t>
template<typename base_renderer_t>
void ndarray_canvas<pixfmt_t>::_draw_shape_internal(VertexSource& shape,
    const agg::trans_affine& transform, Paint& linePaint, Paint& fillPaint,
    const GraphicsState& gs, base_renderer_t& renderer)
{
    typedef agg::conv_transform<VertexSource> conv_trans_t;
    typedef agg::conv_contour<conv_trans_t> contour_shape_t;
    typedef agg::scanline_u8 scanline_t;

    const GraphicsState::DrawingMode mode = gs.drawing_mode();
    const bool line = (mode & GraphicsState::DrawStroke) == GraphicsState::DrawStroke;
    const bool fill = (mode & GraphicsState::DrawFill) == GraphicsState::DrawFill;
    const bool eof = (mode & GraphicsState::DrawEofFill) == GraphicsState::DrawEofFill;

    if (line || fill)
    {
        _set_aa(gs.anti_aliased());
        agg::trans_affine mtx = transform;
        conv_trans_t trans_shape(shape, mtx);
        scanline_t scanline;

        if (fill)
        {
            contour_shape_t contour(trans_shape);
            contour.auto_detect_orientation(true);

            m_rasterizer.reset();
            m_rasterizer.add_path(contour);
            m_rasterizer.filling_rule(eof ? agg::fill_even_odd : agg::fill_non_zero);
            fillPaint.render<pixfmt_t, rasterizer_t, scanline_t, base_renderer_t>(m_rasterizer, scanline, renderer, mtx);
        }

        if (line)
        {
            // Handle dashing and other such details
            _draw_shape_stroke_setup(trans_shape, mtx, linePaint, gs, renderer);
        }
    }
}

template<typename pixfmt_t>
template<typename shape_t, typename base_renderer_t>
void ndarray_canvas<pixfmt_t>::_draw_shape_stroke_setup(shape_t& shape,
    agg::trans_affine& mtx, Paint& paint, const GraphicsState& gs,
    base_renderer_t& renderer)
{
    typedef agg::conv_dash<shape_t> dash_t;
    typedef agg::conv_stroke<dash_t> dash_stroke_t;
    typedef agg::conv_stroke<shape_t> stroke_t;

    if (gs.line_dash_pattern().size() > 0)
    {
        typedef GraphicsState::DashPattern::size_type counter_t;

        dash_t dash(shape);
        dash_stroke_t stroke(dash);
        const GraphicsState::DashPattern& dashPattern = gs.line_dash_pattern();

        for (counter_t i=0; i < dashPattern.size(); i+=2)
            dash.add_dash(dashPattern[i], dashPattern[i+1]);
        dash.dash_start(0.0);

        _draw_shape_stroke_final(stroke, mtx, paint, gs, renderer);
    }
    else
    {
        stroke_t stroke(shape);
        _draw_shape_stroke_final(stroke, mtx, paint, gs, renderer);
    }
}

template<typename pixfmt_t>
template<typename stroke_t, typename base_renderer_t>
void ndarray_canvas<pixfmt_t>::_draw_shape_stroke_final(stroke_t& stroke,
    agg::trans_affine& mtx, Paint& paint, const GraphicsState& gs,
    base_renderer_t& renderer)
{
    typedef agg::scanline_u8 scanline_t;
    scanline_t scanline;

    stroke.width(gs.line_width());
    stroke.miter_limit(gs.miter_limit());
    stroke.inner_miter_limit(gs.inner_miter_limit());
    stroke.line_cap(agg::line_cap_e(gs.line_cap()));
    stroke.line_join(agg::line_join_e(gs.line_join()));
    stroke.inner_join(agg::inner_join_e(gs.inner_join()));

    m_rasterizer.reset();
    m_rasterizer.add_path(stroke);
    paint.render<pixfmt_t, rasterizer_t, scanline_t, base_renderer_t>(m_rasterizer, scanline, renderer, mtx);
}

template<typename pixfmt_t>
template<typename base_renderer_t>
void ndarray_canvas<pixfmt_t>::_draw_text_internal(const char* text, Font& font,
    const agg::trans_affine& transform, Paint& linePaint, Paint& fillPaint,
    const GraphicsState& gs, base_renderer_t& renderer)
{
    agg::trans_affine mtx = transform;
    double transform_array[6];
    double start_x, start_y;
    const bool font_flip = font.flip();

    // Flip the font?
    font.flip(!m_bottom_up);

    // Pull the translation values out of the matrix as our starting offset and
    // then replace them with zeros for use in the font engine.
    mtx.store_to(transform_array);
    start_x = transform_array[4];
    start_y = transform_array[5];
    transform_array[4] = 0.0;
    transform_array[5] = 0.0;
    mtx.load_from(transform_array);
    font.transform(mtx);

    GlyphIterator iterator(text, font, true, start_x, start_y);
    if (font.cache_type() == Font::RasterFontCache)
    {
        _draw_text_raster(iterator, font, mtx, linePaint, fillPaint, gs, renderer);
    }
    else
    {
        agg::trans_affine identity;
        // XXX: TextDrawingMode only applies to Vector fonts!
        GraphicsState copy_state(gs);
        copy_state.drawing_mode(_convert_text_mode(gs.text_drawing_mode()));
        _draw_text_vector(iterator, font, identity, linePaint, fillPaint, copy_state, renderer);
    }

    // Restore the font's flip state to whatever it was
    font.flip(font_flip);
}

template<typename pixfmt_t>
template<typename base_renderer_t>
void ndarray_canvas<pixfmt_t>::_draw_text_raster(GlyphIterator& iterator,
    Font& font, agg::trans_affine& transform, Paint& linePaint, Paint& fillPaint,
    const GraphicsState& gs, base_renderer_t& renderer)
{
#ifdef _ENABLE_TEXT_RENDERING
    typedef Font::FontCacheManager::gray8_adaptor_type font_rasterizer_t;
    typedef Font::FontCacheManager::gray8_scanline_type scanline_t;

    font_rasterizer_t& ras = font.cache().gray8_adaptor();
    scanline_t& scanline = font.cache().gray8_scanline();

    const bool eof = (gs.drawing_mode() & GraphicsState::DrawEofFill) == GraphicsState::DrawEofFill;
    m_rasterizer.filling_rule(eof ? agg::fill_even_odd : agg::fill_non_zero);

    GlyphIterator::StepAction action = GlyphIterator::k_StepActionInvalid;
    while (action != GlyphIterator::k_StepActionEnd)
    {
        if (action == GlyphIterator::k_StepActionDraw)
        {
            linePaint.render<pixfmt_t, font_rasterizer_t, scanline_t, base_renderer_t>(ras, scanline, renderer, transform);
        }
        action = iterator.step();
    }
#endif
}

template<typename pixfmt_t>
template<typename base_renderer_t>
void ndarray_canvas<pixfmt_t>::_draw_text_vector(GlyphIterator& iterator,
    Font& font, agg::trans_affine& transform, Paint& linePaint, Paint& fillPaint,
    const GraphicsState& gs, base_renderer_t& renderer)
{
#ifdef _ENABLE_TEXT_RENDERING
    typedef agg::conv_transform<Font::FontCacheManager::path_adaptor_type> trans_font_t;

    GlyphIterator::StepAction action = GlyphIterator::k_StepActionInvalid;
    while (action != GlyphIterator::k_StepActionEnd)
    {
        if (action == GlyphIterator::k_StepActionDraw)
        {
            trans_font_t tr(font.cache().path_adaptor(), transform);
            PathSource shape;
            shape.concat_path(tr, 0);

            _draw_shape_internal(shape, transform, linePaint, fillPaint, gs, renderer);
        }
        action = iterator.step();
    }
#endif
}

template<typename pixfmt_t>
GraphicsState::DrawingMode ndarray_canvas<pixfmt_t>::_convert_text_mode(const GraphicsState::TextDrawingMode tm)
{
    // XXX: None of the clip drawing modes are implemented as clipping
    switch (tm)
    {
        case GraphicsState::TextDrawInvisible:
            return GraphicsState::DrawInvisible;
        case GraphicsState::TextDrawClip:
        case GraphicsState::TextDrawFill:
        case GraphicsState::TextDrawFillClip:
            return GraphicsState::DrawFill;
        case GraphicsState::TextDrawStroke:
        case GraphicsState::TextDrawStrokeClip:
            return GraphicsState::DrawStroke;
        case GraphicsState::TextDrawFillStroke:
        case GraphicsState::TextDrawFillStrokeClip:
            return GraphicsState::DrawFillStroke;
        default:
            return GraphicsState::DrawInvisible;
    }
}

template<typename pixfmt_t>
void ndarray_canvas<pixfmt_t>::_set_aa(const bool& aa)
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
void ndarray_canvas<pixfmt_t>::_set_clipping(const GraphicsState::Rect& rect)
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
