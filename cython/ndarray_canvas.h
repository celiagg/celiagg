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

#pragma once

#include "agg_conv_contour.h"
#include "agg_conv_stroke.h"
#include "agg_path_storage.h"
#include "agg_pixfmt_rgb.h"
#include "agg_rasterizer_scanline_aa.h"
#include "agg_renderer_base.h"
#include "agg_renderer_scanline.h"
#include "agg_rendering_buffer.h"
#include "agg_scanline_p.h"
#include <cstdint>

class ndarray_canvas
{
public:
    ndarray_canvas() = delete;
    ndarray_canvas(std::uint8_t* buf, unsigned width, unsigned height, int stride);
    unsigned width() const;
    unsigned height() const;

    void draw_line(double x0, double y0, double x1, double y1, double w, std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a, bool aa);
    void draw_polygon(const double* points, size_t point_count,
                      bool outline, double outline_w,
                      std::uint8_t outline_r, std::uint8_t outline_g, std::uint8_t outline_b, std::uint8_t outline_a,
                      bool fill,
                      std::uint8_t fill_r,    std::uint8_t fill_g,    std::uint8_t fill_b,    std::uint8_t fill_a,
                      bool aa);

protected:
    typedef agg::pixfmt_rgbx32 pixfmt_t;
    typedef agg::renderer_base<pixfmt_t> rendererbase_t;
    typedef agg::renderer_scanline_aa_solid<rendererbase_t> renderer_t;

    agg::rendering_buffer m_renbuf;
    pixfmt_t m_pixfmt;
    rendererbase_t m_rendererbase;
    renderer_t m_renderer;

    agg::rasterizer_scanline_aa<> m_rasterizer;
    agg::scanline_p8 m_scanline;

    inline void set_aa(const bool& aa);
};
