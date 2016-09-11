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

#include "image.h"

// Specializations!
template<> PixelFormat get_pixel_format<agg::pixfmt_gray8>() { return k_PixelFormatGray8; }
template<> PixelFormat get_pixel_format<agg::pixfmt_sgray8>() { return k_PixelFormatSGray8; }
template<> PixelFormat get_pixel_format<agg::pixfmt_gray16>() { return k_PixelFormatGray16; }
template<> PixelFormat get_pixel_format<agg::pixfmt_gray32>() { return k_PixelFormatGray32; }
template<> PixelFormat get_pixel_format<agg::pixfmt_bgr24>() { return k_PixelFormatBGR24; }
template<> PixelFormat get_pixel_format<agg::pixfmt_rgb24>() { return k_PixelFormatRGB24; }
template<> PixelFormat get_pixel_format<agg::pixfmt_sbgr24>() { return k_PixelFormatSBGR24; }
template<> PixelFormat get_pixel_format<agg::pixfmt_srgb24>() { return k_PixelFormatSRGB24; }
template<> PixelFormat get_pixel_format<agg::pixfmt_bgr48>() { return k_PixelFormatBGR48; }
template<> PixelFormat get_pixel_format<agg::pixfmt_rgb48>() { return k_PixelFormatRGB48; }
template<> PixelFormat get_pixel_format<agg::pixfmt_bgr96>() { return k_PixelFormatBGR96; }
template<> PixelFormat get_pixel_format<agg::pixfmt_rgb96>() { return k_PixelFormatRGB96; }
template<> PixelFormat get_pixel_format<agg::pixfmt_bgra32>() { return k_PixelFormatBGRA32; }
template<> PixelFormat get_pixel_format<agg::pixfmt_rgba32>() { return k_PixelFormatRGBA32; }
template<> PixelFormat get_pixel_format<agg::pixfmt_argb32>() { return k_PixelFormatARGB32; }
template<> PixelFormat get_pixel_format<agg::pixfmt_abgr32>() { return k_PixelFormatABGR32; }
template<> PixelFormat get_pixel_format<agg::pixfmt_sbgra32>() { return k_PixelFormatSBGRA32; }
template<> PixelFormat get_pixel_format<agg::pixfmt_srgba32>() { return k_PixelFormatSRGBA32; }
template<> PixelFormat get_pixel_format<agg::pixfmt_sargb32>() { return k_PixelFormatSARGB32; }
template<> PixelFormat get_pixel_format<agg::pixfmt_sabgr32>() { return k_PixelFormatSABGR32; }
template<> PixelFormat get_pixel_format<agg::pixfmt_bgra64>() { return k_PixelFormatBGRA64; }
template<> PixelFormat get_pixel_format<agg::pixfmt_rgba64>() { return k_PixelFormatRGBA64; }
template<> PixelFormat get_pixel_format<agg::pixfmt_argb64>() { return k_PixelFormatARGB64; }
template<> PixelFormat get_pixel_format<agg::pixfmt_abgr64>() { return k_PixelFormatABGR64; }
template<> PixelFormat get_pixel_format<agg::pixfmt_bgra128>() { return k_PixelFormatBGRA128; }
template<> PixelFormat get_pixel_format<agg::pixfmt_rgba128>() { return k_PixelFormatRGBA128; }
template<> PixelFormat get_pixel_format<agg::pixfmt_argb128>() { return k_PixelFormatARGB128; }
template<> PixelFormat get_pixel_format<agg::pixfmt_abgr128>() { return k_PixelFormatABGR128; }

Image::Image(uint8_t* buf, unsigned width, unsigned height, int stride,
             const PixelFormat format)
: m_buf(buf, width, height, stride)
, m_format(format)
{
}

agg::rendering_buffer& Image::get_buffer()
{
    return m_buf;
}

agg::path_storage Image::image_outline() const
{
    agg::path_storage path;
    path.move_to(0, 0);
    path.line_to(width(), 0);
    path.line_to(width(), height());
    path.line_to(0, height());
    path.close_polygon();
    return path;
}

unsigned Image::height() const
{
    return m_buf.height();
}

unsigned Image::width() const
{
    return m_buf.width();
}
