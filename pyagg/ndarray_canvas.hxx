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
 

template<typename pixfmt_T, typename value_type_T>
ndarray_canvas<pixfmt_T, value_type_T>::ndarray_canvas(value_type_T* buf,
                                         const unsigned& width, const unsigned& height, const int& stride,
                                         const size_t& channel_count)
  : m_channel_count(channel_count),
    m_renbuf(buf, width, height, stride),
    m_pixfmt(m_renbuf),
    m_rendererbase(m_pixfmt),
    m_renderer(m_rendererbase)
{
    // Clipping at the rasterizer level (vectorial) is far faster than clipping at the renderer level (pixel-wise) when 
    // significant clipping is required.  Clipping in the renderer is done by throwing away pixel data falling outside 
    // any clipping box, whereas the rasterizer clipping avoids generating pixels outside of its current clip box.
    m_rasterizer.clip_box(0, 0, width, height);
}

template<typename pixfmt_T, typename value_type_T>
const size_t& ndarray_canvas<pixfmt_T, value_type_T>::channel_count() const
{
//  typedef typename pixfmt_T::order_type T;
//  return T::N;
    // Something like the above should work, but the cop-out of just storing channel count 
    // ourselves definitely works. 
    return m_channel_count;
}

template<typename pixfmt_T, typename value_type_T>
unsigned ndarray_canvas<pixfmt_T, value_type_T>::width() const
{
    return m_renbuf.width();
}

template<typename pixfmt_T, typename value_type_T>
unsigned ndarray_canvas<pixfmt_T, value_type_T>::height() const
{
    return m_renbuf.height();
}

template<typename pixfmt_T, typename value_type_T>
void ndarray_canvas<pixfmt_T, value_type_T>::draw_path(const agg::path_storage& path,
    const agg::trans_affine& transform, const GraphicsState& gs)
{
    typedef agg::conv_transform<agg::path_storage> conv_trans_t;
    typedef agg::conv_curve<conv_trans_t> conv_curve_t;
    typedef agg::conv_contour<conv_curve_t> contour_curve_t;
    typedef agg::conv_stroke<conv_curve_t> stroke_curve_t;

    const GraphicsState::DrawingMode mode = gs.drawingMode();
    const bool line = (mode & GraphicsState::DrawStroke) == GraphicsState::DrawStroke;
    const bool fill = (mode & GraphicsState::DrawFill) == GraphicsState::DrawFill;
    const bool eof = (mode & GraphicsState::DrawEofFill) == GraphicsState::DrawEofFill;

    if (line || fill)
    {
        set_aa(gs.antiAliased());
        agg::path_storage path_copy(path);
        agg::trans_affine mtx = transform;
        conv_trans_t trans_path(path_copy, mtx);
        conv_curve_t curve(trans_path);

        if (fill)
        {
            contour_curve_t contour(curve);
            contour.auto_detect_orientation(true);

            m_rasterizer.reset();
            m_rasterizer.add_path(contour);
            m_renderer.color(color_from_srgba8<pixfmt_T>(gs.fillColor()));
            m_rasterizer.filling_rule(eof ? agg::fill_even_odd : agg::fill_non_zero);
            agg::render_scanlines(m_rasterizer, m_scanline, m_renderer);
        }

        if (line)
        {
            stroke_curve_t stroke(curve);
            stroke.width(gs.lineWidth());

            m_rasterizer.reset();
            m_rasterizer.add_path(stroke);
            m_renderer.color(color_from_srgba8<pixfmt_T>(gs.lineColor()));
            agg::render_scanlines(m_rasterizer, m_scanline, m_renderer);
        }
    }
}

template<typename pixfmt_T, typename value_type_T>
void ndarray_canvas<pixfmt_T, value_type_T>::draw_bspline(const double* points,
    const size_t& point_count, const agg::trans_affine& transform, const GraphicsState& gs)
{
    typedef agg::conv_transform<agg::simple_polygon_vertex_source> trans_verts_t;
    typedef agg::conv_bspline<trans_verts_t> conv_bspline_t;
    typedef agg::conv_contour<conv_bspline_t> contour_bspline_t;
    typedef agg::conv_stroke<conv_bspline_t> stroke_bspline_t;

    const GraphicsState::DrawingMode mode = gs.drawingMode();
    const bool line = (mode & GraphicsState::DrawStroke) == GraphicsState::DrawStroke;
    const bool fill = (mode & GraphicsState::DrawFill) == GraphicsState::DrawFill;
    const bool eof = (mode & GraphicsState::DrawEofFill) == GraphicsState::DrawEofFill;

    if (line || fill)
    {
        set_aa(gs.antiAliased());
        agg::simple_polygon_vertex_source path(points, point_count, false, false);
        agg::trans_affine mtx = transform;
        trans_verts_t trans_verts(path, mtx);
        conv_bspline_t bspline(trans_verts);
//      bspline.interpolation_step(1.0 / point_count);

        if(fill)
        {
            contour_bspline_t contour(bspline);
            contour.auto_detect_orientation(true);

            m_rasterizer.reset();
            m_rasterizer.add_path(contour);
            m_renderer.color(color_from_srgba8<pixfmt_T>(gs.fillColor()));
            m_rasterizer.filling_rule(eof ? agg::fill_even_odd : agg::fill_non_zero);
            agg::render_scanlines(m_rasterizer, m_scanline, m_renderer);
        }

        if(line)
        {
            stroke_bspline_t stroke(bspline);
            stroke.width(gs.lineWidth() / 2);

            m_rasterizer.reset();
            m_rasterizer.add_path(stroke);
            m_renderer.color(color_from_srgba8<pixfmt_T>(gs.lineColor()));
            agg::render_scanlines(m_rasterizer, m_scanline, m_renderer);
        }
    }
}

template<typename pixfmt_T, typename value_type_T>
void ndarray_canvas<pixfmt_T, value_type_T>::draw_image(Image& img,
    const agg::trans_affine& transform, const GraphicsState& gs)
{
    typedef typename image_filters<pixfmt_T>::nearest_type span_gen_type;
    typedef typename image_filters<pixfmt_T>::source_type source_type;
    typedef typename agg::span_allocator<typename pixfmt_T::color_type> span_alloc_type;
    typedef agg::renderer_scanline_aa<rendererbase_t, span_alloc_type, span_gen_type> renderer_type;
    typedef agg::conv_transform<agg::path_storage> trans_curve_t;

    agg::path_storage img_outline = img.image_outline();
    agg::rendering_buffer* src_buf = img.rendering_buffer_ptr();
    pixfmt_T src_pix(*src_buf);

    agg::trans_affine src_mtx = transform;
    agg::trans_affine inv_img_mtx = transform;
    inv_img_mtx.invert();
    interpolator_type interpolator(inv_img_mtx);

    typename pixfmt_T::color_type back_color = color_from_srgba8<pixfmt_T>(agg::srgba8(128, 128, 128, 0));
    source_type source(src_pix, back_color);
    span_gen_type span_generator(source, interpolator);
    span_alloc_type span_allocator;
    renderer_type renderer(m_rendererbase, span_allocator, span_generator);

    m_rasterizer.reset();
    trans_curve_t trans_outline(img_outline, src_mtx);
    m_rasterizer.add_path(trans_outline);
    agg::render_scanlines(m_rasterizer, m_scanline, renderer);
}

template<typename pixfmt_T, typename value_type_T>
void ndarray_canvas<pixfmt_T, value_type_T>::draw_text(const char* text,
    Font& font, const agg::trans_affine& transform, const GraphicsState& gs)
{
    agg::trans_affine mtx = transform;
    double transform_array[6];
    double start_x, start_y;

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
        _draw_text_raster(iterator, font, gs);
    }
    else
    {
        _draw_text_vector(iterator, font, mtx, gs);
    }

}

template<typename pixfmt_T, typename value_type_T>
void ndarray_canvas<pixfmt_T, value_type_T>::_draw_text_raster(GlyphIterator& iterator,
    Font& font, const GraphicsState& gs)
{
    m_rasterizer.filling_rule(agg::fill_non_zero);
    if ((gs.drawingMode() & GraphicsState::DrawFill) == GraphicsState::DrawFill)
    {
        m_renderer.color(color_from_srgba8<pixfmt_T>(gs.fillColor()));
    }
    else
    {
        m_renderer.color(color_from_srgba8<pixfmt_T>(gs.lineColor()));
    }

    GlyphIterator::StepAction action = GlyphIterator::k_StepActionInvalid;
    while (action != GlyphIterator::k_StepActionEnd)
    {
        if (action == GlyphIterator::k_StepActionDraw)
        {
            agg::render_scanlines(font.cache().gray8_adaptor(),
                                    font.cache().gray8_scanline(),
                                    m_renderer);
        }
        action = iterator.step();
    }
}

template<typename pixfmt_T, typename value_type_T>
void ndarray_canvas<pixfmt_T, value_type_T>::_draw_text_vector(GlyphIterator& iterator,
    Font& font, agg::trans_affine& transform, const GraphicsState& gs)
{
    typedef agg::conv_transform<Font::FontCacheManager::path_adaptor_type> trans_font_t;

    GlyphIterator::StepAction action = GlyphIterator::k_StepActionInvalid;
    while (action != GlyphIterator::k_StepActionEnd)
    {
        if (action == GlyphIterator::k_StepActionDraw)
        {
            trans_font_t tr(font.cache().path_adaptor(), transform);
            agg::path_storage path;

            path.concat_path(tr, 0);
            draw_path(path, transform, gs);
        }
        action = iterator.step();
    }
}

template<typename pixfmt_T, typename value_type_T>
void ndarray_canvas<pixfmt_T, value_type_T>::set_aa(const bool& aa)
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
