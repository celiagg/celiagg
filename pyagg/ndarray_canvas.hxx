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
void ndarray_canvas<pixfmt_T, value_type_T>::draw_path(const agg::path_storage& path, const GraphicsState& gs)
{
    const bool line = (gs.drawingMode() & GraphicsState::DrawStroke) == GraphicsState::DrawStroke;
    const bool fill = (gs.drawingMode() & GraphicsState::DrawFill) == GraphicsState::DrawFill;

    if (line || fill)
    {
        set_aa(gs.antiAliased());
        agg::path_storage path_copy(path);
        path_copy.close_polygon();
        typedef agg::conv_curve<agg::path_storage> conv_curve_t;
        conv_curve_t curve(path_copy);

        if (fill)
        {
            agg::conv_contour<conv_curve_t> contour(curve);
            contour.auto_detect_orientation(true);
            m_rasterizer.reset();
            m_rasterizer.add_path(contour);
            m_renderer.color(color_from_srgba8<pixfmt_T>(gs.fillColor()));
            agg::render_scanlines(m_rasterizer, m_scanline, m_renderer);
        }

        if (line)
        {
            agg::conv_stroke<conv_curve_t> stroke(curve);
            stroke.width(gs.lineWidth() / 2);
            m_rasterizer.reset();
            m_rasterizer.add_path(stroke);
            m_renderer.color(color_from_srgba8<pixfmt_T>(gs.lineColor()));
            agg::render_scanlines(m_rasterizer, m_scanline, m_renderer);
        }
    }
}

template<typename pixfmt_T, typename value_type_T>
void ndarray_canvas<pixfmt_T, value_type_T>::draw_bspline(const double* points, const size_t& point_count,
                                                          const GraphicsState& gs)
{
    const bool line = (gs.drawingMode() & GraphicsState::DrawStroke) == GraphicsState::DrawStroke;
    const bool fill = (gs.drawingMode() & GraphicsState::DrawFill) == GraphicsState::DrawFill;

    if(line || fill)
    {
        set_aa(gs.antiAliased());
        agg::simple_polygon_vertex_source path(points, point_count, false, false);
        typedef agg::conv_bspline<agg::simple_polygon_vertex_source> conv_bspline_t;
        conv_bspline_t bspline(path);
//      bspline.interpolation_step(1.0 / point_count);

        if(fill)
        {
            agg::conv_contour<conv_bspline_t> contour(bspline);
            contour.auto_detect_orientation(true);
            m_rasterizer.reset();
            m_rasterizer.add_path(contour);
            m_renderer.color(color_from_srgba8<pixfmt_T>(gs.fillColor()));
            agg::render_scanlines(m_rasterizer, m_scanline, m_renderer);
        }

        if(line)
        {
            agg::conv_stroke<conv_bspline_t> stroke(bspline);
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
    const double x, const double y, const GraphicsState& gs)
{
    typedef typename image_filters<pixfmt_T>::nearest_type span_gen_type;
    typedef typename image_filters<pixfmt_T>::source_type source_type;
    typedef typename agg::span_allocator<typename pixfmt_T::color_type> span_alloc_type;
    typedef agg::renderer_scanline_aa<rendererbase_t, span_alloc_type, span_gen_type> renderer_type;
    typedef agg::conv_transform<agg::path_storage> trans_curve_t;

    agg::path_storage img_outline = img.image_outline();
    agg::rendering_buffer* src_buf = img.rendering_buffer_ptr();
    pixfmt_T src_pix(*src_buf);

    agg::trans_affine src_mtx;
    src_mtx *= agg::trans_affine_translation(x, y);

    agg::trans_affine inv_img_mtx;
    inv_img_mtx *= agg::trans_affine_translation(x, y);
    inv_img_mtx.invert();
    interpolator_type interpolator(inv_img_mtx);

    typename pixfmt_T::color_type back_color = color_from_srgba8<pixfmt_T>(agg::srgba8(128, 128, 128, 128));
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
    const double x, const double y, Font& font, const GraphicsState& gs)
{
    double start_x = x;
    double start_y = y;

    agg::trans_affine  mtx;
    mtx *= agg::trans_affine_translation(x, y);
    agg::conv_transform<Font::FontCacheManager::path_adaptor_type> tr(font.cache().path_adaptor(), mtx);
    agg::path_storage path;

    for (int i = 0; text[i]; i++)
    {
        const agg::glyph_cache* glyph = font.cache().glyph(text[i]);
        if (glyph)
        {
            if (i > 0)
            {
                font.cache().add_kerning(&start_x, &start_y);
            }
            font.cache().init_embedded_adaptors(glyph, start_x, start_y);

            if (glyph->data_type == agg::glyph_data_outline)
            {
                path.remove_all();
                path.concat_path(tr, 0);
                draw_path(path, gs);
            }
            if (glyph->data_type == agg::glyph_data_gray8)
            {
                agg::render_scanlines(font.cache().gray8_adaptor(),
                                      font.cache().gray8_scanline(),
                                      m_renderer);
            }
            start_x += glyph->advance_x;
            start_y += glyph->advance_y;
        }
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
