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
 

template<typename pixfmt_t>
ndarray_canvas<pixfmt_t>::ndarray_canvas(uint8_t* buf,
    const unsigned width, const unsigned height, const int stride,
    const size_t channel_count, const bool bottom_up)
: m_channel_count(channel_count)
, m_renbuf(buf, width, height, bottom_up ? -stride : stride)
, m_pixfmt(m_renbuf)
, m_renderer(m_pixfmt)
, m_bottom_up(bottom_up)
{
    // Clipping at the rasterizer level (vectorial) is far faster than clipping at the renderer level (pixel-wise) when 
    // significant clipping is required.  Clipping in the renderer is done by throwing away pixel data falling outside 
    // any clipping box, whereas the rasterizer clipping avoids generating pixels outside of its current clip box.
    m_rasterizer.clip_box(0, 0, width, height);
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
    if (gs.stencil() == NULL)
    {
        _draw_image_internal(img, transform, gs, m_renderer);
    }
    else
    {
        masked_renderer_t renderer = _get_masked_renderer(gs);
        _draw_image_internal(img, transform, gs, renderer);
    }
}

template<typename pixfmt_t>
void ndarray_canvas<pixfmt_t>::draw_shape(VertexSource& shape,
    const agg::trans_affine& transform, Paint& linePaint, Paint& fillPaint,
    const GraphicsState& gs)
{
    if (gs.stencil() == NULL)
    {
        _draw_shape_internal(shape, transform, linePaint, fillPaint, gs, m_renderer);
    }
    else
    {
        masked_renderer_t renderer = _get_masked_renderer(gs);
        _draw_shape_internal(shape, transform, linePaint, fillPaint, gs, renderer);
    }
}

template<typename pixfmt_t>
void ndarray_canvas<pixfmt_t>::draw_text(const char* text,
    Font& font, const agg::trans_affine& transform,
    Paint& linePaint, Paint& fillPaint, const GraphicsState& gs)
{
    if (gs.stencil() == NULL)
    {
        _draw_text_internal(text, font, transform, linePaint, fillPaint, gs, m_renderer);
    }
    else
    {
        masked_renderer_t renderer = _get_masked_renderer(gs);
        _draw_text_internal(text, font, transform, linePaint, fillPaint, gs, renderer);
    }
}

template<typename pixfmt_t>
template<typename base_renderer_t>
void ndarray_canvas<pixfmt_t>::_draw_image_internal(Image& img,
    const agg::trans_affine& transform, const GraphicsState& gs,
    base_renderer_t& renderer)
{
    typedef typename image_filters<pixfmt_t>::nearest_t span_gen_t;
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

    const GraphicsState::DrawingMode mode = gs.drawingMode();
    const bool line = (mode & GraphicsState::DrawStroke) == GraphicsState::DrawStroke;
    const bool fill = (mode & GraphicsState::DrawFill) == GraphicsState::DrawFill;
    const bool eof = (mode & GraphicsState::DrawEofFill) == GraphicsState::DrawEofFill;

    if (line || fill)
    {
        _set_aa(gs.antiAliased());
        agg::trans_affine mtx = transform;
        conv_trans_t trans_shape(shape, mtx);

        if (fill)
        {
            contour_shape_t contour(trans_shape);
            contour.auto_detect_orientation(true);

            m_rasterizer.reset();
            m_rasterizer.add_path(contour);
            m_rasterizer.filling_rule(eof ? agg::fill_even_odd : agg::fill_non_zero);
            fillPaint.render<pixfmt_t, rasterizer_t, base_renderer_t>(m_rasterizer, renderer, mtx);
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

    if (gs.lineDashPattern().size() > 0)
    {
        typedef GraphicsState::DashPattern::size_type counter_t;

        dash_t dash(shape);
        dash_stroke_t stroke(dash);
        const GraphicsState::DashPattern& dashPattern = gs.lineDashPattern();

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
    stroke.width(gs.lineWidth());
    stroke.line_cap(agg::line_cap_e(gs.lineCap()));
    stroke.line_join(agg::line_join_e(gs.lineJoin()));

    m_rasterizer.reset();
    m_rasterizer.add_path(stroke);
    paint.render<pixfmt_t, rasterizer_t, base_renderer_t>(m_rasterizer, renderer, mtx);
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

    GlyphIterator iterator(text, font, true, start_x, start_y);
    if (font.cacheType() == Font::RasterFontCache)
    {
        font.transform(mtx);
        _draw_text_raster(iterator, font, mtx, linePaint, fillPaint, gs, renderer);
    }
    else
    {
        _draw_text_vector(iterator, font, mtx, linePaint, fillPaint, gs, renderer);
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
    typedef Font::FontCacheManager::gray8_adaptor_type font_rasterizer_t;
    const bool eof = (gs.drawingMode() & GraphicsState::DrawEofFill) == GraphicsState::DrawEofFill;
    m_rasterizer.filling_rule(eof ? agg::fill_even_odd : agg::fill_non_zero);

    GlyphIterator::StepAction action = GlyphIterator::k_StepActionInvalid;
    while (action != GlyphIterator::k_StepActionEnd)
    {
        if (action == GlyphIterator::k_StepActionDraw)
        {
            linePaint.render<pixfmt_t, font_rasterizer_t, base_renderer_t>(font.cache().gray8_adaptor(), renderer, transform);
        }
        action = iterator.step();
    }
}

template<typename pixfmt_t>
template<typename base_renderer_t>
void ndarray_canvas<pixfmt_t>::_draw_text_vector(GlyphIterator& iterator,
    Font& font, agg::trans_affine& transform, Paint& linePaint, Paint& fillPaint,
    const GraphicsState& gs, base_renderer_t& renderer)
{
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
}

template<typename pixfmt_t>
typename ndarray_canvas<pixfmt_t>::masked_renderer_t
ndarray_canvas<pixfmt_t>::_get_masked_renderer(const GraphicsState& gs)
{
    Image* stencil = const_cast<Image*>(gs.stencil());
    agg::rendering_buffer& stencilbuf = stencil->get_buffer();
    alpha_mask_t stencil_mask(stencilbuf);
    masked_pxfmt_t masked_pixfmt(m_pixfmt, stencil_mask);
    return masked_renderer_t(masked_pixfmt);
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
