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
    _set_clipping(gs.clip_box());
    // XXX: Apply master alpha here somehow!

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
void ndarray_canvas<pixfmt_t>::draw_shape_at_points(VertexSource& shape,
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
void ndarray_canvas<pixfmt_t>::draw_text(const char* text,
    Font& font, const agg::trans_affine& transform,
    Paint& linePaint, Paint& fillPaint, const GraphicsState& gs)
{
    _set_clipping(gs.clip_box());
    linePaint.master_alpha(gs.master_alpha());
    fillPaint.master_alpha(gs.master_alpha());

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
template<typename base_renderer_t>
void ndarray_canvas<pixfmt_t>::_draw_image_internal(Image& img,
    const agg::trans_affine& transform, const GraphicsState& gs,
    base_renderer_t& renderer)
{
    typedef typename image_filters<pixfmt_t>::source_t source_t;
    typedef agg::conv_transform<agg::path_storage> trans_curve_t;

    pixfmt_t src_pix(img.get_buffer());
    typename pixfmt_t::color_type back_color(agg::rgba(0.5, 0.5, 0.5, 1.0));
    source_t source(src_pix, back_color);

    agg::trans_affine src_mtx = transform;
    agg::trans_affine inv_img_mtx = ~src_mtx;
    interpolator_t interpolator(inv_img_mtx);

    m_rasterizer.reset();
    agg::path_storage img_outline = img.image_outline();
    trans_curve_t trans_outline(img_outline, src_mtx);
    m_rasterizer.add_path(trans_outline);

    switch (gs.image_interpolation_mode())
    {
    case GraphicsState::InterpolationNearest:
        {
            typedef typename image_filters<pixfmt_t>::nearest_t span_gen_t;

            span_gen_t span_generator(source, interpolator);
            _draw_image_final<base_renderer_t>(renderer, span_generator);
            break;
        }
    case GraphicsState::InterpolationBilinear:
        {
            typedef typename image_filters<pixfmt_t>::bilinear_t span_gen_t;

            span_gen_t span_generator(source, interpolator);
            _draw_image_final<base_renderer_t>(renderer, span_generator);
            break;
        }
    case GraphicsState::InterpolationBicubic:
    case GraphicsState::InterpolationSpline16:
    case GraphicsState::InterpolationSpline36:
    case GraphicsState::InterpolationSinc64:
    case GraphicsState::InterpolationSinc144:
    case GraphicsState::InterpolationSinc256:
    case GraphicsState::InterpolationBlackman64:
    case GraphicsState::InterpolationBlackman100:
    case GraphicsState::InterpolationBlackman256:
        {
            typedef typename image_filters<pixfmt_t>::general_t span_gen_t;
            agg::image_filter_lut filter;
            switch (gs.image_interpolation_mode())
            {
                case GraphicsState::InterpolationBicubic:
                    filter.calculate(agg::image_filter_bicubic()); break;
                case GraphicsState::InterpolationSpline16:
                    filter.calculate(agg::image_filter_spline16()); break;
                case GraphicsState::InterpolationSpline36:
                    filter.calculate(agg::image_filter_spline36()); break;
                case GraphicsState::InterpolationSinc64:
                    filter.calculate(agg::image_filter_sinc64()); break;
                case GraphicsState::InterpolationSinc144:
                    filter.calculate(agg::image_filter_sinc144()); break;
                case GraphicsState::InterpolationSinc256:
                    filter.calculate(agg::image_filter_sinc256()); break;
                case GraphicsState::InterpolationBlackman64:
                    filter.calculate(agg::image_filter_blackman64()); break;
                case GraphicsState::InterpolationBlackman100:
                    filter.calculate(agg::image_filter_blackman100()); break;
                case GraphicsState::InterpolationBlackman256:
                    filter.calculate(agg::image_filter_blackman256()); break;
                default: break;
            }

            span_gen_t span_generator(source, interpolator, filter);
            _draw_image_final<base_renderer_t>(renderer, span_generator);
            break;
        }
    }
}

template<typename pixfmt_t>
template<typename base_renderer_t, typename span_gen_t>
void ndarray_canvas<pixfmt_t>::_draw_image_final(
    base_renderer_t& renderer, span_gen_t& span_generator)
{
    typedef typename agg::span_allocator<typename pixfmt_t::color_type> span_alloc_t;
    typedef agg::renderer_scanline_aa<base_renderer_t, span_alloc_t, span_gen_t> img_renderer_t;

    span_alloc_t span_allocator;
    img_renderer_t img_renderer(renderer, span_allocator, span_generator);
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
        scanline_t scanline;

        if (fill)
        {
            agg::trans_affine mtx = transform;
            conv_trans_t trans_shape(shape, mtx);
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
            _draw_shape_stroke_setup(shape, transform, linePaint, gs, renderer);
        }
    }
}

template<typename pixfmt_t>
template<typename base_renderer_t>
void ndarray_canvas<pixfmt_t>::_draw_shape_stroke_setup(VertexSource& shape,
    const agg::trans_affine& mtx, Paint& paint, const GraphicsState& gs,
    base_renderer_t& renderer)
{
    typedef agg::conv_dash<VertexSource> dash_t;
    typedef agg::conv_stroke<dash_t> dash_stroke_t;
    typedef agg::conv_stroke<VertexSource> stroke_t;

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
    const agg::trans_affine& mtx, Paint& paint, const GraphicsState& gs,
    base_renderer_t& renderer)
{
    typedef agg::conv_transform<stroke_t> trans_stroke_t;
    typedef agg::scanline_u8 scanline_t;
    scanline_t scanline;

    stroke.width(gs.line_width());
    stroke.miter_limit(gs.miter_limit());
    stroke.inner_miter_limit(gs.inner_miter_limit());
    stroke.line_cap(agg::line_cap_e(gs.line_cap()));
    stroke.line_join(agg::line_join_e(gs.line_join()));
    stroke.inner_join(agg::inner_join_e(gs.inner_join()));

    agg::trans_affine src_mtx = mtx;
    trans_stroke_t trans(stroke, src_mtx);

    m_rasterizer.reset();
    m_rasterizer.add_path(trans);
    paint.render<pixfmt_t, rasterizer_t, scanline_t, base_renderer_t>(m_rasterizer, scanline, renderer, mtx);
}

template<typename pixfmt_t>
template<typename base_renderer_t>
void ndarray_canvas<pixfmt_t>::_draw_text_internal(const char* text, Font& font,
    const agg::trans_affine& transform, Paint& linePaint, Paint& fillPaint,
    const GraphicsState& gs, base_renderer_t& renderer)
{
    // Flip the font?
    const bool font_flip = font.flip();
    font.flip(!m_bottom_up);

    GlyphIterator iterator(text, m_font_cache, true);
    if (gs.text_drawing_mode() == GraphicsState::TextDrawRaster)
    {
        // Raster text only uses the fill paint!
        _draw_text_raster(iterator, font, transform, fillPaint, gs, renderer);
    }
    else
    {
        // Pick the correct drawing mode for the glyph paths
        GraphicsState copy_state(gs);
        copy_state.drawing_mode(_convert_text_mode(gs.text_drawing_mode()));
        _draw_text_vector(iterator, font, transform, linePaint, fillPaint, copy_state, renderer);
    }

    // Restore the font's flip state to whatever it was
    font.flip(font_flip);
}

template<typename pixfmt_t>
template<typename base_renderer_t>
void ndarray_canvas<pixfmt_t>::_draw_text_raster(GlyphIterator& iterator,
    Font& font, const agg::trans_affine& transform, Paint& fillPaint,
    const GraphicsState& gs, base_renderer_t& renderer)
{
#ifdef _ENABLE_TEXT_RENDERING
    typedef FontCache::FontCacheManager::gray8_adaptor_type font_rasterizer_t;
    typedef FontCache::FontCacheManager::gray8_scanline_type scanline_t;

    const bool eof = (gs.drawing_mode() & GraphicsState::DrawEofFill) == GraphicsState::DrawEofFill;
    m_rasterizer.filling_rule(eof ? agg::fill_even_odd : agg::fill_non_zero);

    // Strip the translation out of the transformation when activating the font
    // so that the cached glyphs aren't associated with a specific translation,
    // only the current rotation & shear.
    double transform_array[6];
    transform.store_to(transform_array);
    transform_array[4] = 0.0; transform_array[5] = 0.0;
    m_font_cache.activate(font, agg::trans_affine(transform_array), FontCache::k_GlyphTypeRaster);

    // Determine the starting glyph position from the transform and initialize
    // the iterator.
    double start_x = 0.0, start_y = 0.0;
    transform.transform(&start_x, &start_y);
    iterator.offset(start_x, start_y);

    // Rendery bits from the font cache
    font_rasterizer_t& ras = m_font_cache.manager().gray8_adaptor();
    scanline_t& scanline = m_font_cache.manager().gray8_scanline();

    // Draw the glyphs one at a time
    GlyphIterator::StepAction action = GlyphIterator::k_StepActionInvalid;
    while (action != GlyphIterator::k_StepActionEnd)
    {
        if (action == GlyphIterator::k_StepActionDraw)
        {
            fillPaint.render<pixfmt_t, font_rasterizer_t, scanline_t, base_renderer_t>(ras, scanline, renderer, transform);
        }
        action = iterator.step();
    }
#endif
}

template<typename pixfmt_t>
template<typename base_renderer_t>
void ndarray_canvas<pixfmt_t>::_draw_text_vector(GlyphIterator& iterator,
    Font& font, const agg::trans_affine& transform, Paint& linePaint, Paint& fillPaint,
    const GraphicsState& gs, base_renderer_t& renderer)
{
#ifdef _ENABLE_TEXT_RENDERING
    PathSource shape;

    // Activate the font with an identity transform. The passed in transform
    // will be applied later when drawing the generated path.
    m_font_cache.activate(font, agg::trans_affine(), FontCache::k_GlyphTypeVector);

    GlyphIterator::StepAction action = GlyphIterator::k_StepActionInvalid;
    while (action != GlyphIterator::k_StepActionEnd)
    {
        if (action == GlyphIterator::k_StepActionDraw)
        {
            shape.concat_path(m_font_cache.manager().path_adaptor());
        }
        action = iterator.step();
    }
    _draw_shape_internal(shape, transform, linePaint, fillPaint, gs, renderer);
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
