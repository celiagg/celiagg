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
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// 
// Authors: John Wiggins

#ifndef PYAGG_IMAGE_H 
#define PYAGG_IMAGE_H 

#include <stdint.h>

#include <agg_image_accessors.h>
#include <agg_path_storage.h>
#include <agg_pixfmt_gray.h>
#include <agg_pixfmt_rgb.h>
#include <agg_pixfmt_rgba.h>
#include <agg_rendering_buffer.h>
#include <agg_scanline_u.h>
#include <agg_span_allocator.h>
#include <agg_span_image_filter_gray.h>
#include <agg_span_image_filter_rgb.h>
#include <agg_span_image_filter_rgba.h>
#include <agg_span_interpolator_linear.h>
#include <agg_span_pattern_gray.h>
#include <agg_span_pattern_rgb.h>
#include <agg_span_pattern_rgba.h>


typedef agg::span_interpolator_linear<> interpolator_t;
typedef agg::wrap_mode_reflect wrap_reflect_t;
typedef agg::wrap_mode_repeat wrap_repeat_t;

template<typename pixfmt_t>
struct image_filters {};

template<>
struct image_filters<agg::pixfmt_rgba128>
{
    typedef agg::image_accessor_wrap<agg::pixfmt_rgba128, wrap_reflect_t, wrap_reflect_t> pattern_source_reflect_t;
    typedef agg::image_accessor_wrap<agg::pixfmt_rgba128, wrap_repeat_t, wrap_repeat_t> pattern_source_repeat_t;
    typedef agg::image_accessor_clip<agg::pixfmt_rgba128> source_t;
    typedef agg::span_image_filter_rgba_nn<source_t,  interpolator_t> nearest_t;
    typedef agg::span_image_filter_rgba_bilinear<source_t, interpolator_t> bilinear_t;
    typedef agg::span_image_filter_rgba<source_t, interpolator_t> general_t;
    typedef agg::span_pattern_rgba<pattern_source_reflect_t> pattern_reflect_span_gen_t;
    typedef agg::span_pattern_rgba<pattern_source_repeat_t> pattern_repeat_span_gen_t;
};

template<>
struct image_filters<agg::pixfmt_rgba32>
{
    typedef agg::image_accessor_wrap<agg::pixfmt_rgba32, wrap_reflect_t, wrap_reflect_t> pattern_source_reflect_t;
    typedef agg::image_accessor_wrap<agg::pixfmt_rgba32, wrap_repeat_t, wrap_repeat_t> pattern_source_repeat_t;
    typedef agg::image_accessor_clip<agg::pixfmt_rgba32> source_t;
    typedef agg::span_image_filter_rgba_nn<source_t,  interpolator_t> nearest_t;
    typedef agg::span_image_filter_rgba_bilinear<source_t, interpolator_t> bilinear_t;
    typedef agg::span_image_filter_rgba<source_t, interpolator_t> general_t;
    typedef agg::span_pattern_rgba<pattern_source_reflect_t> pattern_reflect_span_gen_t;
    typedef agg::span_pattern_rgba<pattern_source_repeat_t> pattern_repeat_span_gen_t;
};

template<>
struct image_filters<agg::pixfmt_bgra32>
{
    typedef agg::image_accessor_wrap<agg::pixfmt_bgra32, wrap_reflect_t, wrap_reflect_t> pattern_source_reflect_t;
    typedef agg::image_accessor_wrap<agg::pixfmt_bgra32, wrap_repeat_t, wrap_repeat_t> pattern_source_repeat_t;
    typedef agg::image_accessor_clip<agg::pixfmt_bgra32> source_t;
    typedef agg::span_image_filter_rgba_nn<source_t, interpolator_t> nearest_t;
    typedef agg::span_image_filter_rgba_bilinear<source_t, interpolator_t> bilinear_t;
    typedef agg::span_image_filter_rgba<source_t, interpolator_t> general_t;
    typedef agg::span_pattern_rgba<pattern_source_reflect_t> pattern_reflect_span_gen_t;
    typedef agg::span_pattern_rgba<pattern_source_repeat_t> pattern_repeat_span_gen_t;
};

template<>
struct image_filters<agg::pixfmt_argb32>
{
    typedef agg::image_accessor_wrap<agg::pixfmt_argb32, wrap_reflect_t, wrap_reflect_t> pattern_source_reflect_t;
    typedef agg::image_accessor_wrap<agg::pixfmt_argb32, wrap_repeat_t, wrap_repeat_t> pattern_source_repeat_t;
    typedef agg::image_accessor_clip<agg::pixfmt_argb32> source_t;
    typedef agg::span_image_filter_rgba_nn<source_t, interpolator_t> nearest_t;
    typedef agg::span_image_filter_rgba_bilinear<source_t, interpolator_t> bilinear_t;
    typedef agg::span_image_filter_rgba<source_t, interpolator_t> general_t;
    typedef agg::span_pattern_rgba<pattern_source_reflect_t> pattern_reflect_span_gen_t;
    typedef agg::span_pattern_rgba<pattern_source_repeat_t> pattern_repeat_span_gen_t;
};

template<>
struct image_filters<agg::pixfmt_abgr32>
{
    typedef agg::image_accessor_wrap<agg::pixfmt_abgr32, wrap_reflect_t, wrap_reflect_t> pattern_source_reflect_t;
    typedef agg::image_accessor_wrap<agg::pixfmt_abgr32, wrap_repeat_t, wrap_repeat_t> pattern_source_repeat_t;
    typedef agg::image_accessor_clip<agg::pixfmt_abgr32> source_t;
    typedef agg::span_image_filter_rgba_nn<source_t, interpolator_t> nearest_t;
    typedef agg::span_image_filter_rgba_bilinear<source_t, interpolator_t> bilinear_t;
    typedef agg::span_image_filter_rgba<source_t, interpolator_t> general_t;
    typedef agg::span_pattern_rgba<pattern_source_reflect_t> pattern_reflect_span_gen_t;
    typedef agg::span_pattern_rgba<pattern_source_repeat_t> pattern_repeat_span_gen_t;
};

template<>
struct image_filters<agg::pixfmt_rgb24>
{
    typedef agg::image_accessor_wrap<agg::pixfmt_rgb24, wrap_reflect_t, wrap_reflect_t> pattern_source_reflect_t;
    typedef agg::image_accessor_wrap<agg::pixfmt_rgb24, wrap_repeat_t, wrap_repeat_t> pattern_source_repeat_t;
    typedef agg::image_accessor_clip<agg::pixfmt_rgb24> source_t;
    typedef agg::span_image_filter_rgb_nn<source_t, interpolator_t> nearest_t;
    typedef agg::span_image_filter_rgb_bilinear<source_t, interpolator_t> bilinear_t;
    typedef agg::span_image_filter_rgb<source_t, interpolator_t> general_t;
    typedef agg::span_pattern_rgb<pattern_source_reflect_t> pattern_reflect_span_gen_t;
    typedef agg::span_pattern_rgb<pattern_source_repeat_t> pattern_repeat_span_gen_t;
};

template<>
struct image_filters<agg::pixfmt_bgr24>
{
    typedef agg::image_accessor_wrap<agg::pixfmt_bgr24, wrap_reflect_t, wrap_reflect_t> pattern_source_reflect_t;
    typedef agg::image_accessor_wrap<agg::pixfmt_bgr24, wrap_repeat_t, wrap_repeat_t> pattern_source_repeat_t;
    typedef agg::image_accessor_clip<agg::pixfmt_bgr24> source_t;
    typedef agg::span_image_filter_rgb_nn<source_t, interpolator_t> nearest_t;
    typedef agg::span_image_filter_rgb_bilinear<source_t, interpolator_t> bilinear_t;
    typedef agg::span_image_filter_rgb<source_t, interpolator_t> general_t;
    typedef agg::span_pattern_rgb<pattern_source_reflect_t> pattern_reflect_span_gen_t;
    typedef agg::span_pattern_rgb<pattern_source_repeat_t> pattern_repeat_span_gen_t;
};

template<>
struct image_filters<agg::pixfmt_gray16>
{
    typedef agg::image_accessor_wrap<agg::pixfmt_gray16, wrap_reflect_t, wrap_reflect_t> pattern_source_reflect_t;
    typedef agg::image_accessor_wrap<agg::pixfmt_gray16, wrap_repeat_t, wrap_repeat_t> pattern_source_repeat_t;
    typedef agg::image_accessor_clip<agg::pixfmt_gray16> source_t;
    typedef agg::span_image_filter_gray_nn<source_t, interpolator_t> nearest_t;
    typedef agg::span_image_filter_gray_bilinear<source_t, interpolator_t> bilinear_t;
    typedef agg::span_image_filter_gray<source_t, interpolator_t> general_t;
    typedef agg::span_pattern_gray<pattern_source_reflect_t> pattern_reflect_span_gen_t;
    typedef agg::span_pattern_gray<pattern_source_repeat_t> pattern_repeat_span_gen_t;
};

template<>
struct image_filters<agg::pixfmt_gray8>
{
    typedef agg::image_accessor_wrap<agg::pixfmt_gray8, wrap_reflect_t, wrap_reflect_t> pattern_source_reflect_t;
    typedef agg::image_accessor_wrap<agg::pixfmt_gray8, wrap_repeat_t, wrap_repeat_t> pattern_source_repeat_t;
    typedef agg::image_accessor_clip<agg::pixfmt_gray8> source_t;
    typedef agg::span_image_filter_gray_nn<source_t, interpolator_t> nearest_t;
    typedef agg::span_image_filter_gray_bilinear<source_t, interpolator_t> bilinear_t;
    typedef agg::span_image_filter_gray<source_t, interpolator_t> general_t;
    typedef agg::span_pattern_gray<pattern_source_reflect_t> pattern_reflect_span_gen_t;
    typedef agg::span_pattern_gray<pattern_source_repeat_t> pattern_repeat_span_gen_t;
};


class Image
{
    agg::rendering_buffer m_buf;

public:
    Image(uint8_t* buf, unsigned width, unsigned height, int stride) :
        m_buf(buf, width, height, stride) {}

    agg::rendering_buffer* get_buffer_ptr() { return &m_buf; }
    unsigned height() const { return m_buf.height(); }
    unsigned width() const { return m_buf.width(); }
    int stride() const { return m_buf.stride(); }

    agg::path_storage image_outline() const
    {
        agg::path_storage path;
        path.move_to(0, 0);
        path.line_to(m_buf.width(), 0);
        path.line_to(m_buf.width(), m_buf.height());
        path.line_to(0, m_buf.height());
        path.close_polygon();
        return path;
    }
};

#endif // PYAGG_IMAGE_H 
