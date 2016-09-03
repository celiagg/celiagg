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

#include <agg_image_accessors.h>
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


typedef agg::span_interpolator_linear<> interpolator_t;

template<typename pixel_format>
struct image_filters {};

template<>
struct image_filters<agg::pixfmt_rgba32>
{
    typedef agg::image_accessor_clip<agg::pixfmt_rgba32> source_type;
    typedef agg::span_image_filter_rgba_nn<source_type,  interpolator_t> nearest_type;
    typedef agg::span_image_filter_rgba_bilinear<source_type, interpolator_t> bilinear_type;
    typedef agg::span_image_filter_rgba<source_type, interpolator_t> general_type;                   
};

template<>
struct image_filters<agg::pixfmt_bgra32>
{
    typedef agg::image_accessor_clip<agg::pixfmt_bgra32> source_type;
    typedef agg::span_image_filter_rgba_nn<source_type, interpolator_t> nearest_type;
    typedef agg::span_image_filter_rgba_bilinear<source_type, interpolator_t> bilinear_type;
    typedef agg::span_image_filter_rgba<source_type, interpolator_t> general_type;                   
};

template<>
struct image_filters<agg::pixfmt_argb32>
{
    typedef agg::image_accessor_clip<agg::pixfmt_argb32> source_type;
    typedef agg::span_image_filter_rgba_nn<source_type, interpolator_t> nearest_type;
    typedef agg::span_image_filter_rgba_bilinear<source_type, interpolator_t> bilinear_type;
    typedef agg::span_image_filter_rgba<source_type, interpolator_t> general_type;                   
};

template<>
struct image_filters<agg::pixfmt_abgr32>
{
    typedef agg::image_accessor_clip<agg::pixfmt_abgr32> source_type;
    typedef agg::span_image_filter_rgba_nn<source_type, interpolator_t> nearest_type;
    typedef agg::span_image_filter_rgba_bilinear<source_type, interpolator_t> bilinear_type;
    typedef agg::span_image_filter_rgba<source_type, interpolator_t> general_type;                   
};

template<>
struct image_filters<agg::pixfmt_rgb24>
{
    typedef agg::image_accessor_clip<agg::pixfmt_rgb24> source_type;
    typedef agg::span_image_filter_rgb_nn<source_type, interpolator_t> nearest_type;
    typedef agg::span_image_filter_rgb_bilinear<source_type, interpolator_t> bilinear_type;
    typedef agg::span_image_filter_rgb<source_type, interpolator_t> general_type;                   
};

template<>
struct image_filters<agg::pixfmt_bgr24>
{
    typedef agg::image_accessor_clip<agg::pixfmt_bgr24> source_type;
    typedef agg::span_image_filter_rgb_nn<source_type, interpolator_t> nearest_type;
    typedef agg::span_image_filter_rgb_bilinear<source_type, interpolator_t> bilinear_type;
    typedef agg::span_image_filter_rgb<source_type, interpolator_t> general_type;                   
};

template<>
struct image_filters<agg::pixfmt_gray16>
{
    typedef agg::image_accessor_clip<agg::pixfmt_gray16> source_type;
    typedef agg::span_image_filter_gray_nn<source_type, interpolator_t> nearest_type;
    typedef agg::span_image_filter_gray_bilinear<source_type, interpolator_t> bilinear_type;
    typedef agg::span_image_filter_gray<source_type, interpolator_t> general_type;                   
};

template<>
struct image_filters<agg::pixfmt_gray8>
{
    typedef agg::image_accessor_clip<agg::pixfmt_gray8> source_type;
    typedef agg::span_image_filter_gray_nn<source_type, interpolator_t> nearest_type;
    typedef agg::span_image_filter_gray_bilinear<source_type, interpolator_t> bilinear_type;
    typedef agg::span_image_filter_gray<source_type, interpolator_t> general_type;                   
};


class Image
{
    agg::rendering_buffer renBuf;

public:
    Image(unsigned char* buf, unsigned width, unsigned height, int stride) :
        renBuf(buf, width, height, stride) {}
    agg::rendering_buffer* rendering_buffer_ptr() { return &renBuf; }
    int width()  const { return renBuf.width(); }
    int height() const { return renBuf.height(); }
    agg::path_storage image_outline() const
    {
        agg::path_storage path;
        path.move_to(0, 0);
        path.line_to(width(), 0);
        path.line_to(width(), height());
        path.line_to(0, height());
        path.close_polygon();
        return path;
    }
};


#endif // PYAGG_IMAGE_H 
