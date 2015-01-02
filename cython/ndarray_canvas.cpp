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

#include "ndarray_canvas.h"

ndarray_canvas::ndarray_canvas(std::uint8_t* buf, unsigned width, unsigned height, int stride)
  : m_renbuf(buf, width, height, stride),
    m_pixfmt(m_renbuf),
    m_rendererbase(m_pixfmt),
    m_renderer(m_rendererbase)
{
}

unsigned ndarray_canvas::width() const
{
    return m_renbuf.width();
}

unsigned ndarray_canvas::height() const
{
    return m_renbuf.height();
}

void ndarray_canvas::draw_line(double x0, double y0, double x1, double y1, double w, std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a, bool aa)
{
    set_aa(aa);
    agg::path_storage path;
    path.move_to(x0, y0);
    path.line_to(x1, y1);
    agg::conv_stroke<agg::path_storage> stroke(path);
    stroke.width(w / 2);
    m_rasterizer.reset();
    m_rasterizer.add_path(stroke);
    m_renderer.color(agg::rgba8(r, g, b, a));
    agg::render_scanlines(m_rasterizer, m_scanline, m_renderer);
}

void ndarray_canvas::draw_polygon(const double* points, size_t point_count,
                                  bool outline, double outline_w,
                                  std::uint8_t outline_r, std::uint8_t outline_g, std::uint8_t outline_b, std::uint8_t outline_a,
                                  bool fill,
                                  std::uint8_t fill_r,    std::uint8_t fill_g,    std::uint8_t fill_b,    std::uint8_t fill_a,
                                  bool aa)
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
        path.close_polygon();

        if(fill)
        {
            agg::conv_contour<agg::path_storage> contour(path);
            contour.auto_detect_orientation(true);
            contour.width(outline ? outline_w / 2 : 0.5);
            m_rasterizer.reset();
            m_rasterizer.add_path(contour);
            m_renderer.color(agg::rgba8(fill_r, fill_g, fill_b, fill_a));
            agg::render_scanlines(m_rasterizer, m_scanline, m_renderer);
        }

        if(outline)
        {
            agg::conv_stroke<agg::path_storage> stroke(path);
            stroke.width(outline_w / 2);
            m_rasterizer.reset();
            m_rasterizer.add_path(stroke);
            m_renderer.color(agg::rgba8(outline_r, outline_g, outline_b, outline_a));
            agg::render_scanlines(m_rasterizer, m_scanline, m_renderer);
        }
    }
}

void ndarray_canvas::set_aa(const bool& aa)
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
