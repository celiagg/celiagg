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

template<typename pixfmt_t>
template<typename base_renderer_t>
void canvas<pixfmt_t>::_draw_text_internal(const char* text, Font& font,
    const agg::trans_affine& transform, Paint& linePaint, Paint& fillPaint,
    const GraphicsState& gs, base_renderer_t& renderer)
{
    // Flip the font?
    const bool font_flip = font.flip();
    font.flip(!m_bottom_up);

    if (gs.text_drawing_mode() == GraphicsState::TextDrawRaster)
    {
        // Raster text only uses the fill paint!
        _draw_text_raster(text, font, transform, fillPaint, gs, renderer);
    }
    else
    {
        // Pick the correct drawing mode for the glyph paths
        GraphicsState copy_state(gs);
        copy_state.drawing_mode(_convert_text_mode(gs.text_drawing_mode()));
        _draw_text_vector(text, font, transform, linePaint, fillPaint, copy_state, renderer);
    }

    // Restore the font's flip state to whatever it was
    font.flip(font_flip);
}

template<typename pixfmt_t>
template<typename base_renderer_t>
void canvas<pixfmt_t>::_draw_text_color_raster(const char* text,
    Font& font, const agg::trans_affine& transform, const GraphicsState& gs,
    base_renderer_t& renderer)
{
    typedef typename image_filters<pixfmt_t>::source_t source_t;
    typedef typename agg::span_allocator<typename pixfmt_t::color_type> span_alloc_t;
    typedef typename image_filters<pixfmt_t>::nearest_t span_gen_t;
    typedef agg::renderer_scanline_aa<base_renderer_t, span_alloc_t, span_gen_t> img_renderer_t;
    typedef agg::conv_transform<agg::path_storage> trans_curve_t;

#ifdef _ENABLE_TEXT_RENDERING
    typedef FontCache::shaper_type shaper_t;

    // Activate the font with an identity transform. The passed in transform
    // will be applied later when drawing the glyphs
    m_font_cache.activate(font, agg::trans_affine(), FontCache::k_GlyphTypeColorRaster);

    // Rendery bits from the font cache
    shaper_t& shaper = m_font_cache.shaper();

    // Shape the text
    shaper.shape(text);

    // Glyph image rendering pieces
    typename pixfmt_t::color_type back_color(agg::rgba(0.5, 0.5, 0.5, 1.0));
    agg::trans_affine src_mtx = transform;
    interpolator_t interpolator(~transform);
    agg::path_storage glyph_outline;
    span_alloc_t span_allocator;

    // Draw the glyphs one at a time
    Shaper::StepAction action = Shaper::k_StepActionSkip;
    while (action != Shaper::k_StepActionEnd)
    {
        action = shaper.step();
        if (action == Shaper::k_StepActionDraw)
        {
            const agg::glyph_cache* glyph = m_font_cache.manager().last_glyph();
            const unsigned width = glyph->bounds.x2;
            const unsigned height = glyph->bounds.y2;
            const unsigned stride = glyph->data_size / height;

            agg::rendering_buffer buffer(glyph->data, width, height, stride);
            pixfmt_t src_pix(buffer);
            source_t source(src_pix, back_color);
            span_gen_t span_generator(source, interpolator);
            img_renderer_t img_renderer(renderer, span_allocator, span_generator);

            glyph_outline.remove_all();
            glyph_outline.move_to(0, 0);
            glyph_outline.line_to(width, 0);
            glyph_outline.line_to(width, height;
            glyph_outline.line_to(0, height);
            glyph_outline.close_polygon();
            m_rasterizer.reset();
            m_rasterizer.add_path(trans_curve_t(glyph_outline, src_mtx));
            agg::render_scanlines(m_rasterizer, m_scanline, img_renderer);

        }
    }
#endif
}

template<typename pixfmt_t>
template<typename base_renderer_t>
void canvas<pixfmt_t>::_draw_text_raster(const char* text,
    Font& font, const agg::trans_affine& transform, Paint& fillPaint,
    const GraphicsState& gs, base_renderer_t& renderer)
{
#ifdef _ENABLE_TEXT_RENDERING
    typedef FontCache::FontCacheManager::gray8_adaptor_type font_rasterizer_t;
    typedef FontCache::FontCacheManager::gray8_scanline_type scanline_t;
    typedef FontCache::shaper_type shaper_t;

    // Strip the translation out of the transformation when activating the font
    // so that the cached glyphs aren't associated with a specific translation,
    // only the current rotation & shear.
    double transform_array[6];
    transform.store_to(transform_array);
    transform_array[4] = 0.0; transform_array[5] = 0.0;
    m_font_cache.activate(font, agg::trans_affine(transform_array), FontCache::k_GlyphTypeRaster);

    // Rendery bits from the font cache
    font_rasterizer_t& ras = m_font_cache.manager().gray8_adaptor();
    scanline_t& scanline = m_font_cache.manager().gray8_scanline();
    shaper_t& shaper = m_font_cache.shaper();

    // Determine the starting glyph position from the transform
    double start_x = 0.0, start_y = 0.0;
    transform.transform(&start_x, &start_y);

    // Shape the text and initialize the starting position
    shaper.shape(text);
    // Set cursor position last, because shape() resets it
    shaper.cursor(start_x, start_y);

    // Draw the glyphs one at a time
    Shaper::StepAction action = Shaper::k_StepActionSkip;
    while (action != Shaper::k_StepActionEnd)
    {
        action = shaper.step();
        if (action == Shaper::k_StepActionDraw)
        {
            const agg::glyph_cache* glyph = m_font_cache.manager().last_glyph();
            m_font_cache.manager().init_embedded_adaptors(glyph, shaper.cursor_x(), shaper.cursor_y());
            fillPaint.render<pixfmt_t, font_rasterizer_t, scanline_t, base_renderer_t>(ras, scanline, renderer, transform);
        }
    }
#endif
}

template<typename pixfmt_t>
template<typename base_renderer_t>
void canvas<pixfmt_t>::_draw_text_vector(const char* text,
    Font& font, const agg::trans_affine& transform, Paint& linePaint, Paint& fillPaint,
    const GraphicsState& gs, base_renderer_t& renderer)
{
#ifdef _ENABLE_TEXT_RENDERING
    typedef FontCache::shaper_type shaper_t;

    PathSource glyphs_path;
    shaper_t& shaper = m_font_cache.shaper();

    // Activate the font with an identity transform. The passed in transform
    // will be applied later when drawing the generated path.
    m_font_cache.activate(font, agg::trans_affine(), FontCache::k_GlyphTypeVector);

    // Shape the text
    shaper.shape(text);

    Shaper::StepAction action = Shaper::k_StepActionSkip;
    while (action != Shaper::k_StepActionEnd)
    {
        action = shaper.step();
        if (action == Shaper::k_StepActionDraw)
        {
            const agg::glyph_cache* glyph = m_font_cache.manager().last_glyph();
            m_font_cache.manager().init_embedded_adaptors(glyph, shaper.cursor_x(), shaper.cursor_y());
            glyphs_path.concat_path(m_font_cache.manager().path_adaptor());
        }
    }
    _draw_shape_internal(glyphs_path, transform, linePaint, fillPaint, gs, renderer);
#endif
}

template<typename pixfmt_t>
GraphicsState::DrawingMode canvas<pixfmt_t>::_convert_text_mode(const GraphicsState::TextDrawingMode tm)
{
    // XXX: None of the clip drawing modes are implemented as clipping
    switch (tm)
    {
        case GraphicsState::TextDrawInvisible:
            return GraphicsState::DrawInvisible;
        case GraphicsState::TextDrawClip:
        case GraphicsState::TextDrawFill:
        case GraphicsState::TextDrawFillClip:
        case GraphicsState::TextDrawRaster:
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
