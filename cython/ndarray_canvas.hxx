// The MIT License (MIT)
// 
// Copyright (c) 2014 WUSTL ZPLAB
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
}

template<typename pixfmt_T, typename value_type_T>
const size_t& ndarray_canvas<pixfmt_T, value_type_T>::ndarray_canvas::channel_count() const
{
//  typedef typename pixfmt_T::order_type T;
//  return T::N;
    // Something like the above should work, but the cop-out of just storing channel count 
    // ourselves definitely works. 
    return m_channel_count; 
}

template<typename pixfmt_T, typename value_type_T>
unsigned ndarray_canvas<pixfmt_T, value_type_T>::ndarray_canvas::width() const
{
    return m_renbuf.width();
}

template<typename pixfmt_T, typename value_type_T>
unsigned ndarray_canvas<pixfmt_T, value_type_T>::ndarray_canvas::height() const
{
    return m_renbuf.height();
}

template<typename pixfmt_T, typename value_type_T>
void ndarray_canvas<pixfmt_T, value_type_T>::draw_line(const double& x0, const double& y0,
                                                       const double& x1, const double& y1,
                                                       const double& w,
                                                       const value_type_T* c,
                                                       const bool& aa)
{
    set_aa(aa);
    agg::path_storage path;
    path.move_to(x0, y0);
    path.line_to(x1, y1);
    agg::conv_stroke<agg::path_storage> stroke(path);
    stroke.width(w / 2);
    m_rasterizer.reset();
    m_rasterizer.add_path(stroke);
    m_renderer.color(color_from_channel_array<pixfmt_T>(c));
    agg::render_scanlines(m_rasterizer, m_scanline, m_renderer);
}

template<typename pixfmt_T, typename value_type_T>
void ndarray_canvas<pixfmt_T, value_type_T>::draw_polygon(const double* points, const size_t& point_count,
                                                          const bool& outline, const double& outline_w, const value_type_T* outline_c,
                                                          const bool& fill, const value_type_T* fill_c,
                                                          const bool& aa)
{
    set_aa(aa);
    if(point_count >= 2 && (outline || fill))
    {
        agg::path_storage path;
        const double* points_it{points};
        size_t points_idx{0};
        path.move_to(points_it[0], points_it[1]);
        points_it += 2;
        ++points_idx;
        for(;;)
        {
            path.line_to(points_it[0], points_it[1]);
            ++points_idx;
            if(points_idx == point_count)
            {
                break;
            }
            points_it += 2;
        }

        if(fill)
        {
            agg::conv_contour<agg::path_storage> contour(path);
            contour.auto_detect_orientation(true);
            contour.width(outline ? outline_w / 2 : 0.5);
            m_rasterizer.reset();
            m_rasterizer.add_path(contour);
            m_renderer.color(color_from_channel_array<pixfmt_T>(fill_c));
            agg::render_scanlines(m_rasterizer, m_scanline, m_renderer);
        }

        if(outline)
        {
            agg::conv_stroke<agg::path_storage> stroke(path);
            stroke.width(outline_w / 2);
            m_rasterizer.reset();
            m_rasterizer.add_path(stroke);
            m_renderer.color(color_from_channel_array<pixfmt_T>(outline_c));
            agg::render_scanlines(m_rasterizer, m_scanline, m_renderer);
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
