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
                                                          const bool& line, const double& line_w, const value_type_T* line_c,
                                                          const bool& fill, const value_type_T* fill_c,
                                                          const bool& aa)
{
    set_aa(aa);
    if(point_count >= 2 && (line || fill))
    {
        agg::path_storage path;
        const double* point = points;
        const double*const points_end = point + point_count * 2;
        path.move_to(point[0], point[1]);
        point += 2;
        for(;;)
        {
            path.line_to(point[0], point[1]);
            point += 2;
            if(point >= points_end) break;
        }

        if(fill)
        {
            agg::conv_contour<agg::path_storage> contour(path);
            contour.auto_detect_orientation(true);
            m_rasterizer.reset();
            m_rasterizer.add_path(contour);
            m_renderer.color(color_from_channel_array<pixfmt_T>(fill_c));
            agg::render_scanlines(m_rasterizer, m_scanline, m_renderer);
        }

        if(line)
        {
            agg::conv_stroke<agg::path_storage> stroke(path);
            stroke.width(line_w / 2);
            m_rasterizer.reset();
            m_rasterizer.add_path(stroke);
            m_renderer.color(color_from_channel_array<pixfmt_T>(line_c));
            agg::render_scanlines(m_rasterizer, m_scanline, m_renderer);
        }
    }
}

template<typename pixfmt_T, typename value_type_T>
void ndarray_canvas<pixfmt_T, value_type_T>::draw_ellipse(const double& cx, const double& cy,
                                                          const double& rx, const double& ry,
                                                          const bool& line, const double& line_w, const value_type_T* line_c,
                                                          const bool& fill, const value_type_T* fill_c,
                                                          const bool& aa)
{
    if(line || fill)
    {
        set_aa(aa);
        agg::bezier_arc arc(cx, cy, rx, ry, 0, M_PI + M_PI);
        agg::path_storage path;
        path.concat_path(arc, 0);
        path.close_polygon();
        typedef agg::conv_curve<agg::path_storage> conv_curve_t;
        conv_curve_t curve(path);

        if(fill)
        {
            agg::conv_contour<conv_curve_t> contour(curve);
            contour.auto_detect_orientation(true);
            m_rasterizer.reset();
            m_rasterizer.add_path(contour);
            m_renderer.color(color_from_channel_array<pixfmt_T>(fill_c));
            agg::render_scanlines(m_rasterizer, m_scanline, m_renderer);
        }

        if(line)
        {
            agg::conv_stroke<conv_curve_t> stroke(curve);
            stroke.width(line_w / 2);
            m_rasterizer.reset();
            m_rasterizer.add_path(stroke);
            m_renderer.color(color_from_channel_array<pixfmt_T>(line_c));
            agg::render_scanlines(m_rasterizer, m_scanline, m_renderer);
        }
    }
}

template<typename pixfmt_T, typename value_type_T>
void ndarray_canvas<pixfmt_T, value_type_T>::draw_bezier3(const double& x0, const double& y0,
                                                          const double& x_ctrl, const double& y_ctrl,
                                                          const double& x1, const double& y1,
                                                          const double& w, const value_type_T* c,
                                                          const bool& aa)
{
    set_aa(aa);
    agg::path_storage path;
    path.move_to(x0, y0);
    path.curve3(x_ctrl, y_ctrl, x1, y1);
    typedef agg::conv_curve<agg::path_storage> conv_curve_t;
    conv_curve_t curve(path);
    agg::conv_stroke<conv_curve_t> stroke(curve);
    stroke.width(w / 2);
    m_rasterizer.reset();
    m_rasterizer.add_path(stroke);
    m_renderer.color(color_from_channel_array<pixfmt_T>(c));
    agg::render_scanlines(m_rasterizer, m_scanline, m_renderer);
}

template<typename pixfmt_T, typename value_type_T>
void ndarray_canvas<pixfmt_T, value_type_T>::draw_bezier3_composite(const double* points, const size_t& point_count,
                                                                    const bool& line, const double& line_w, const value_type_T* line_c,
                                                                    const bool& fill, const value_type_T* fill_c,
                                                                    const bool& aa)
{
    if(line || fill)
    {
        set_aa(aa);
        agg::path_storage path;
        const double* point = points;
        const double*const points_end = point + point_count;
        path.move_to(point[0], point[1]);
        point += 2;
        for(;;)
        {
            path.curve3(point[0], point[1], point[2], point[3]);
            point += 4;
            if(point >= points_end) break;
        }
        typedef agg::conv_curve<agg::path_storage> conv_curve_t;
        conv_curve_t curve(path);

        if(fill)
        {
            agg::conv_contour<conv_curve_t> contour(curve);
            contour.auto_detect_orientation(true);
            m_rasterizer.reset();
            m_rasterizer.add_path(contour);
            m_renderer.color(color_from_channel_array<pixfmt_T>(fill_c));
            agg::render_scanlines(m_rasterizer, m_scanline, m_renderer);
        }

        if(line)
        {
            agg::conv_stroke<conv_curve_t> stroke(curve);
            stroke.width(line_w / 2);
            m_rasterizer.reset();
            m_rasterizer.add_path(stroke);
            m_renderer.color(color_from_channel_array<pixfmt_T>(line_c));
            agg::render_scanlines(m_rasterizer, m_scanline, m_renderer);
        }
    }
}

template<typename pixfmt_T, typename value_type_T>
void ndarray_canvas<pixfmt_T, value_type_T>::draw_bezier4(const double& x0, const double& y0,
                                                          const double& x_ctrl0, const double& y_ctrl0,
                                                          const double& x_ctrl1, const double& y_ctrl1,
                                                          const double& x1, const double& y1,
                                                          const double& w, const value_type_T* c,
                                                          const bool& aa)
{
    set_aa(aa);
    agg::path_storage path;
    path.move_to(x0, y0);
    path.curve4(x_ctrl0, y_ctrl0, x_ctrl1, y_ctrl1, x1, y1);
    typedef agg::conv_curve<agg::path_storage> conv_curve_t;
    conv_curve_t curve(path);
    agg::conv_stroke<conv_curve_t> stroke(curve);
    stroke.width(w / 2);
    m_rasterizer.reset();
    m_rasterizer.add_path(stroke);
    m_renderer.color(color_from_channel_array<pixfmt_T>(c));
    agg::render_scanlines(m_rasterizer, m_scanline, m_renderer);
}

template<typename pixfmt_T, typename value_type_T>
void ndarray_canvas<pixfmt_T, value_type_T>::draw_bezier4_composite(const double* points, const size_t& point_count,
                                                                    const bool& line, const double& line_w, const value_type_T* line_c,
                                                                    const bool& fill, const value_type_T* fill_c,
                                                                    const bool& aa)
{
    if(line || fill)
    {
        set_aa(aa);
        agg::path_storage path;
        const double* point = points;
        const double*const points_end = point + point_count;
        path.move_to(point[0], point[1]);
        point += 2;
        for(;;)
        {
            path.curve4(point[0], point[1], point[2], point[3], point[4], point[5]);
            point += 6;
            if(point >= points_end) break;
        }
        typedef agg::conv_curve<agg::path_storage> conv_curve_t;
        conv_curve_t curve(path);

        if(fill)
        {
            agg::conv_contour<conv_curve_t> contour(curve);
            contour.auto_detect_orientation(true);
            m_rasterizer.reset();
            m_rasterizer.add_path(contour);
            m_renderer.color(color_from_channel_array<pixfmt_T>(fill_c));
            agg::render_scanlines(m_rasterizer, m_scanline, m_renderer);
        }

        if(line)
        {
            agg::conv_stroke<conv_curve_t> stroke(curve);
            stroke.width(line_w / 2);
            m_rasterizer.reset();
            m_rasterizer.add_path(stroke);
            m_renderer.color(color_from_channel_array<pixfmt_T>(line_c));
            agg::render_scanlines(m_rasterizer, m_scanline, m_renderer);
        }
    }
}

template<typename pixfmt_T, typename value_type_T>
void ndarray_canvas<pixfmt_T, value_type_T>::draw_bspline(const double* points, const size_t& point_count,
                                                          const bool& line, const double& line_w, const value_type_T* line_c,
                                                          const bool& fill, const value_type_T* fill_c,
                                                          const bool& aa)
{
    if(line || fill)
    {
        set_aa(aa);
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
            m_renderer.color(color_from_channel_array<pixfmt_T>(fill_c));
            agg::render_scanlines(m_rasterizer, m_scanline, m_renderer);
        }

        if(line)
        {
            agg::conv_stroke<conv_bspline_t> stroke(bspline);
            stroke.width(line_w / 2);
            m_rasterizer.reset();
            m_rasterizer.add_path(stroke);
            m_renderer.color(color_from_channel_array<pixfmt_T>(line_c));
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
