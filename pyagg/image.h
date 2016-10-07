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
#include <platform/agg_platform_support.h>
#include <util/agg_color_conv.h>

typedef agg::span_interpolator_linear<> interpolator_t;
typedef agg::wrap_mode_reflect wrap_reflect_t;
typedef agg::wrap_mode_repeat wrap_repeat_t;

template<typename pixfmt_t>
struct image_filters {};

template<>
struct image_filters<agg::pixfmt_rgba128>
{
    typedef agg::pixfmt_rgba128 pixfmt_t;
    typedef agg::image_accessor_clip<pixfmt_t> source_t;
    typedef agg::image_accessor_wrap<pixfmt_t, wrap_reflect_t, wrap_reflect_t> source_reflect_t;
    typedef agg::image_accessor_wrap<pixfmt_t, wrap_repeat_t, wrap_repeat_t> source_repeat_t;
    typedef agg::span_image_filter_rgba_bilinear<source_reflect_t, interpolator_t> bilinear_reflect_t;
    typedef agg::span_image_filter_rgba_bilinear<source_repeat_t, interpolator_t> bilinear_repeat_t;
    typedef agg::span_image_filter_rgba_bilinear<source_t, interpolator_t> bilinear_t;
    typedef agg::span_image_filter_rgba_nn<source_reflect_t,  interpolator_t> nearest_reflect_t;
    typedef agg::span_image_filter_rgba_nn<source_repeat_t,  interpolator_t> nearest_repeat_t;
    typedef agg::span_image_filter_rgba_nn<source_t,  interpolator_t> nearest_t;
    typedef agg::span_image_filter_rgba<source_reflect_t, interpolator_t> general_reflect_t;
    typedef agg::span_image_filter_rgba<source_repeat_t, interpolator_t> general_repeat_t;
    typedef agg::span_image_filter_rgba<source_t, interpolator_t> general_t;
};

template<>
struct image_filters<agg::pixfmt_rgba32>
{
    typedef agg::pixfmt_rgba32 pixfmt_t;
    typedef agg::image_accessor_clip<pixfmt_t> source_t;
    typedef agg::image_accessor_wrap<pixfmt_t, wrap_reflect_t, wrap_reflect_t> source_reflect_t;
    typedef agg::image_accessor_wrap<pixfmt_t, wrap_repeat_t, wrap_repeat_t> source_repeat_t;
    typedef agg::span_image_filter_rgba_bilinear<source_reflect_t, interpolator_t> bilinear_reflect_t;
    typedef agg::span_image_filter_rgba_bilinear<source_repeat_t, interpolator_t> bilinear_repeat_t;
    typedef agg::span_image_filter_rgba_bilinear<source_t, interpolator_t> bilinear_t;
    typedef agg::span_image_filter_rgba_nn<source_reflect_t,  interpolator_t> nearest_reflect_t;
    typedef agg::span_image_filter_rgba_nn<source_repeat_t,  interpolator_t> nearest_repeat_t;
    typedef agg::span_image_filter_rgba_nn<source_t,  interpolator_t> nearest_t;
    typedef agg::span_image_filter_rgba<source_reflect_t, interpolator_t> general_reflect_t;
    typedef agg::span_image_filter_rgba<source_repeat_t, interpolator_t> general_repeat_t;
    typedef agg::span_image_filter_rgba<source_t, interpolator_t> general_t;
};

template<>
struct image_filters<agg::pixfmt_bgra32>
{
    typedef agg::pixfmt_bgra32 pixfmt_t;
    typedef agg::image_accessor_clip<pixfmt_t> source_t;
    typedef agg::image_accessor_wrap<pixfmt_t, wrap_reflect_t, wrap_reflect_t> source_reflect_t;
    typedef agg::image_accessor_wrap<pixfmt_t, wrap_repeat_t, wrap_repeat_t> source_repeat_t;
    typedef agg::span_image_filter_rgba_bilinear<source_reflect_t, interpolator_t> bilinear_reflect_t;
    typedef agg::span_image_filter_rgba_bilinear<source_repeat_t, interpolator_t> bilinear_repeat_t;
    typedef agg::span_image_filter_rgba_bilinear<source_t, interpolator_t> bilinear_t;
    typedef agg::span_image_filter_rgba_nn<source_reflect_t,  interpolator_t> nearest_reflect_t;
    typedef agg::span_image_filter_rgba_nn<source_repeat_t,  interpolator_t> nearest_repeat_t;
    typedef agg::span_image_filter_rgba_nn<source_t, interpolator_t> nearest_t;
    typedef agg::span_image_filter_rgba<source_reflect_t, interpolator_t> general_reflect_t;
    typedef agg::span_image_filter_rgba<source_repeat_t, interpolator_t> general_repeat_t;
    typedef agg::span_image_filter_rgba<source_t, interpolator_t> general_t;
};

template<>
struct image_filters<agg::pixfmt_argb32>
{
    typedef agg::pixfmt_argb32 pixfmt_t;
    typedef agg::image_accessor_clip<pixfmt_t> source_t;
    typedef agg::image_accessor_wrap<pixfmt_t, wrap_reflect_t, wrap_reflect_t> source_reflect_t;
    typedef agg::image_accessor_wrap<pixfmt_t, wrap_repeat_t, wrap_repeat_t> source_repeat_t;
    typedef agg::span_image_filter_rgba_bilinear<source_reflect_t, interpolator_t> bilinear_reflect_t;
    typedef agg::span_image_filter_rgba_bilinear<source_repeat_t, interpolator_t> bilinear_repeat_t;
    typedef agg::span_image_filter_rgba_bilinear<source_t, interpolator_t> bilinear_t;
    typedef agg::span_image_filter_rgba_nn<source_reflect_t,  interpolator_t> nearest_reflect_t;
    typedef agg::span_image_filter_rgba_nn<source_repeat_t,  interpolator_t> nearest_repeat_t;
    typedef agg::span_image_filter_rgba_nn<source_t, interpolator_t> nearest_t;
    typedef agg::span_image_filter_rgba<source_reflect_t, interpolator_t> general_reflect_t;
    typedef agg::span_image_filter_rgba<source_repeat_t, interpolator_t> general_repeat_t;
    typedef agg::span_image_filter_rgba<source_t, interpolator_t> general_t;
};

template<>
struct image_filters<agg::pixfmt_abgr32>
{
    typedef agg::pixfmt_abgr32 pixfmt_t;
    typedef agg::image_accessor_clip<pixfmt_t> source_t;
    typedef agg::image_accessor_wrap<pixfmt_t, wrap_reflect_t, wrap_reflect_t> source_reflect_t;
    typedef agg::image_accessor_wrap<pixfmt_t, wrap_repeat_t, wrap_repeat_t> source_repeat_t;
    typedef agg::span_image_filter_rgba_bilinear<source_reflect_t, interpolator_t> bilinear_reflect_t;
    typedef agg::span_image_filter_rgba_bilinear<source_repeat_t, interpolator_t> bilinear_repeat_t;
    typedef agg::span_image_filter_rgba_bilinear<source_t, interpolator_t> bilinear_t;
    typedef agg::span_image_filter_rgba_nn<source_reflect_t,  interpolator_t> nearest_reflect_t;
    typedef agg::span_image_filter_rgba_nn<source_repeat_t,  interpolator_t> nearest_repeat_t;
    typedef agg::span_image_filter_rgba_nn<source_t, interpolator_t> nearest_t;
    typedef agg::span_image_filter_rgba<source_reflect_t, interpolator_t> general_reflect_t;
    typedef agg::span_image_filter_rgba<source_repeat_t, interpolator_t> general_repeat_t;
    typedef agg::span_image_filter_rgba<source_t, interpolator_t> general_t;
};

template<>
struct image_filters<agg::pixfmt_rgb24>
{
    typedef agg::pixfmt_rgb24 pixfmt_t;
    typedef agg::image_accessor_clip<pixfmt_t> source_t;
    typedef agg::image_accessor_wrap<pixfmt_t, wrap_reflect_t, wrap_reflect_t> source_reflect_t;
    typedef agg::image_accessor_wrap<pixfmt_t, wrap_repeat_t, wrap_repeat_t> source_repeat_t;
    typedef agg::span_image_filter_rgb_bilinear<source_reflect_t, interpolator_t> bilinear_reflect_t;
    typedef agg::span_image_filter_rgb_bilinear<source_repeat_t, interpolator_t> bilinear_repeat_t;
    typedef agg::span_image_filter_rgb_bilinear<source_t, interpolator_t> bilinear_t;
    typedef agg::span_image_filter_rgb_nn<source_reflect_t,  interpolator_t> nearest_reflect_t;
    typedef agg::span_image_filter_rgb_nn<source_repeat_t,  interpolator_t> nearest_repeat_t;
    typedef agg::span_image_filter_rgb_nn<source_t, interpolator_t> nearest_t;
    typedef agg::span_image_filter_rgb<source_reflect_t, interpolator_t> general_reflect_t;
    typedef agg::span_image_filter_rgb<source_repeat_t, interpolator_t> general_repeat_t;
    typedef agg::span_image_filter_rgb<source_t, interpolator_t> general_t;
};

template<>
struct image_filters<agg::pixfmt_bgr24>
{
    typedef agg::pixfmt_bgr24 pixfmt_t;
    typedef agg::image_accessor_clip<pixfmt_t> source_t;
    typedef agg::image_accessor_wrap<pixfmt_t, wrap_reflect_t, wrap_reflect_t> source_reflect_t;
    typedef agg::image_accessor_wrap<pixfmt_t, wrap_repeat_t, wrap_repeat_t> source_repeat_t;
    typedef agg::span_image_filter_rgb_bilinear<source_reflect_t, interpolator_t> bilinear_reflect_t;
    typedef agg::span_image_filter_rgb_bilinear<source_repeat_t, interpolator_t> bilinear_repeat_t;
    typedef agg::span_image_filter_rgb_bilinear<source_t, interpolator_t> bilinear_t;
    typedef agg::span_image_filter_rgb_nn<source_reflect_t,  interpolator_t> nearest_reflect_t;
    typedef agg::span_image_filter_rgb_nn<source_repeat_t,  interpolator_t> nearest_repeat_t;
    typedef agg::span_image_filter_rgb_nn<source_t, interpolator_t> nearest_t;
    typedef agg::span_image_filter_rgb<source_reflect_t, interpolator_t> general_reflect_t;
    typedef agg::span_image_filter_rgb<source_repeat_t, interpolator_t> general_repeat_t;
    typedef agg::span_image_filter_rgb<source_t, interpolator_t> general_t;
};

template<>
struct image_filters<agg::pixfmt_gray16>
{
    typedef agg::pixfmt_gray16 pixfmt_t;
    typedef agg::image_accessor_clip<pixfmt_t> source_t;
    typedef agg::image_accessor_wrap<pixfmt_t, wrap_reflect_t, wrap_reflect_t> source_reflect_t;
    typedef agg::image_accessor_wrap<pixfmt_t, wrap_repeat_t, wrap_repeat_t> source_repeat_t;
    typedef agg::span_image_filter_gray_bilinear<source_reflect_t, interpolator_t> bilinear_reflect_t;
    typedef agg::span_image_filter_gray_bilinear<source_repeat_t, interpolator_t> bilinear_repeat_t;
    typedef agg::span_image_filter_gray_bilinear<source_t, interpolator_t> bilinear_t;
    typedef agg::span_image_filter_gray_nn<source_reflect_t,  interpolator_t> nearest_reflect_t;
    typedef agg::span_image_filter_gray_nn<source_repeat_t,  interpolator_t> nearest_repeat_t;
    typedef agg::span_image_filter_gray_nn<source_t, interpolator_t> nearest_t;
    typedef agg::span_image_filter_gray<source_reflect_t, interpolator_t> general_reflect_t;
    typedef agg::span_image_filter_gray<source_repeat_t, interpolator_t> general_repeat_t;
    typedef agg::span_image_filter_gray<source_t, interpolator_t> general_t;
};

template<>
struct image_filters<agg::pixfmt_gray8>
{
    typedef agg::pixfmt_gray8 pixfmt_t;
    typedef agg::image_accessor_clip<pixfmt_t> source_t;
    typedef agg::image_accessor_wrap<pixfmt_t, wrap_reflect_t, wrap_reflect_t> source_reflect_t;
    typedef agg::image_accessor_wrap<pixfmt_t, wrap_repeat_t, wrap_repeat_t> source_repeat_t;
    typedef agg::span_image_filter_gray_bilinear<source_reflect_t, interpolator_t> bilinear_reflect_t;
    typedef agg::span_image_filter_gray_bilinear<source_repeat_t, interpolator_t> bilinear_repeat_t;
    typedef agg::span_image_filter_gray_bilinear<source_t, interpolator_t> bilinear_t;
    typedef agg::span_image_filter_gray_nn<source_reflect_t,  interpolator_t> nearest_reflect_t;
    typedef agg::span_image_filter_gray_nn<source_repeat_t,  interpolator_t> nearest_repeat_t;
    typedef agg::span_image_filter_gray_nn<source_t, interpolator_t> nearest_t;
    typedef agg::span_image_filter_gray<source_reflect_t, interpolator_t> general_reflect_t;
    typedef agg::span_image_filter_gray<source_repeat_t, interpolator_t> general_repeat_t;
    typedef agg::span_image_filter_gray<source_t, interpolator_t> general_t;
};


enum PixelFormat {
    k_PixelFormatGray8 = agg::pix_format_gray8,
    k_PixelFormatGray16 = agg::pix_format_gray16,
    k_PixelFormatGray32 = agg::pix_format_gray32,
    k_PixelFormatBGR24 = agg::pix_format_bgr24,
    k_PixelFormatRGB24 = agg::pix_format_rgb24,
    k_PixelFormatBGR48 = agg::pix_format_bgr48,
    k_PixelFormatRGB48 = agg::pix_format_rgb48,
    k_PixelFormatBGR96 = agg::pix_format_bgr96,
    k_PixelFormatRGB96 = agg::pix_format_rgb96,
    k_PixelFormatBGRA32 = agg::pix_format_bgra32,
    k_PixelFormatRGBA32 = agg::pix_format_rgba32,
    k_PixelFormatARGB32 = agg::pix_format_argb32,
    k_PixelFormatABGR32 = agg::pix_format_abgr32,
    k_PixelFormatBGRA64 = agg::pix_format_bgra64,
    k_PixelFormatRGBA64 = agg::pix_format_rgba64,
    k_PixelFormatARGB64 = agg::pix_format_argb64,
    k_PixelFormatABGR64 = agg::pix_format_abgr64,
    k_PixelFormatBGRA128 = agg::pix_format_bgra128,
    k_PixelFormatRGBA128 = agg::pix_format_rgba128,
    k_PixelFormatARGB128 = agg::pix_format_argb128,
    k_PixelFormatABGR128 = agg::pix_format_abgr128,
};

class Image
{
    agg::rendering_buffer m_buf;

public:
    Image(uint8_t* buf, unsigned width, unsigned height, int stride);

    agg::rendering_buffer& get_buffer();
    agg::path_storage image_outline() const;
    unsigned height() const;
    unsigned width() const;
};

#endif // PYAGG_IMAGE_H
