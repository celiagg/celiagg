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
#include "agg_conv_curve.h"
#include "agg_conv_stroke.h"
#include "agg_path_storage.h"
#include "agg_pixfmt_gray.h"
#include "agg_pixfmt_rgb.h"
#include "agg_pixfmt_rgba.h"
#include "agg_rasterizer_scanline_aa.h"
#include "agg_renderer_base.h"
#include "agg_renderer_scanline.h"
#include "agg_rendering_buffer.h"
#include "agg_scanline_p.h"
#include <cstdint>

// Interface to ndarray_canvas that is generic for all pixfmts sharing the same value_type, for the convenience of being 
// able to implement functionality common to cython wrappers of ndarray_canvas template instances representing pixfmts 
// with shared value_types in a cython base class. 
template<typename value_type_T>
class ndarray_canvas_base
{
public:
    virtual ~ndarray_canvas_base(){}

    virtual const size_t& channel_count() const = 0;
    virtual unsigned width() const = 0;
    virtual unsigned height() const = 0;

    virtual void draw_line(const double& x0, const double& y0,
                           const double& x1, const double& y1,
                           const double& w,
                           const value_type_T* c,
                           const bool& aa) = 0;

    virtual void draw_polygon(const double* points, const size_t& point_count,
                              const bool& line, const double& line_w, const value_type_T* line_c,
                              const bool& fill, const value_type_T* fill_c,
                              const bool& aa) = 0;

    virtual void draw_bezier3(const double& x0, const double& y0,
                              const double& x_ctrl, const double& y_ctrl,
                              const double& x1, const double& y1,
                              const double& w, const value_type_T* c,
                              const bool& aa) = 0;

    virtual void draw_bezier3_composite(const double* points, const size_t& point_count,
                                        const bool& line, const double& line_w, const value_type_T* line_c,
                                        const bool& fill, const value_type_T* fill_c,
                                        const bool& aa) = 0;

    virtual void draw_bezier4(const double& x0, const double& y0,
                              const double& x_ctrl0, const double& y_ctrl0,
                              const double& x_ctrl1, const double& y_ctrl1,
                              const double& x1, const double& y1,
                              const double& w, const value_type_T* c,
                              const bool& aa) = 0;

    virtual void draw_bezier4_composite(const double* points, const size_t& point_count,
                                        const bool& line, const double& line_w, const value_type_T* line_c,
                                        const bool& fill, const value_type_T* fill_c,
                                        const bool& aa) = 0;
};

template<typename pixfmt_T, typename value_type_T = typename pixfmt_T::value_type>
class ndarray_canvas
  : public ndarray_canvas_base<value_type_T>
{
public:
    ndarray_canvas() = delete;
    ndarray_canvas(value_type_T* buf,
                   const unsigned& width, const unsigned& height, const int& stride,
                   const size_t& channel_count);
    virtual ~ndarray_canvas(){}

    const size_t& channel_count() const;
    unsigned width() const;
    unsigned height() const;

    void draw_line(const double& x0, const double& y0,
                   const double& x1, const double& y1,
                   const double& w,
                   const value_type_T* c,
                   const bool& aa);

    void draw_polygon(const double* points, const size_t& point_count,
                      const bool& line, const double& line_w, const value_type_T* line_c,
                      const bool& fill, const value_type_T* fill_c,
                      const bool& aa);

    void draw_bezier3(const double& x0, const double& y0,
                      const double& x_ctrl, const double& y_ctrl,
                      const double& x1, const double& y1,
                      const double& w, const value_type_T* c,
                      const bool& aa);

    void draw_bezier3_composite(const double* points, const size_t& point_count,
                                const bool& line, const double& line_w, const value_type_T* line_c,
                                const bool& fill, const value_type_T* fill_c,
                                const bool& aa);

    void draw_bezier4(const double& x0, const double& y0,
                      const double& x_ctrl0, const double& y_ctrl0,
                      const double& x_ctrl1, const double& y_ctrl1,
                      const double& x1, const double& y1,
                      const double& w, const value_type_T* c,
                      const bool& aa);

    void draw_bezier4_composite(const double* points, const size_t& point_count,
                                const bool& line, const double& line_w, const value_type_T* line_c,
                                const bool& fill, const value_type_T* fill_c,
                                const bool& aa);

protected:
    typedef agg::renderer_base<pixfmt_T> rendererbase_t;
    typedef agg::renderer_scanline_aa_solid<rendererbase_t> renderer_t;

    size_t m_channel_count;
    agg::row_accessor<value_type_T> m_renbuf;
    pixfmt_T m_pixfmt;
    rendererbase_t m_rendererbase;
    renderer_t m_renderer;

    agg::rasterizer_scanline_aa<> m_rasterizer;
    agg::scanline_p8 m_scanline;

    inline void set_aa(const bool& aa);
};

typedef agg::pixfmt_alpha_blend_gray<agg::blender_gray8, agg::rendering_buffer, 1, 0> pixfmt_gray8_no_alpha;

template<typename pixfmt_T>
typename pixfmt_T::color_type color_from_channel_array(const typename pixfmt_T::value_type* c);

template<>
agg::pixfmt_rgba32::color_type color_from_channel_array<agg::pixfmt_rgba32>(const std::uint8_t* c);

template<>
agg::pixfmt_rgb24::color_type color_from_channel_array<agg::pixfmt_rgb24>(const std::uint8_t* c);

template<>
agg::pixfmt_gray8::color_type color_from_channel_array<agg::pixfmt_gray8>(const std::uint8_t* c);

#include "ndarray_canvas.hxx"

