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

template<typename dst_pixfmt_t, typename src_pixfmt_t>
struct _convert_pixel
{
    void operator()(void* dst, const void* src) const
    {
        const typename src_pixfmt_t::color_type c(src_pixfmt_t::read_plain_color(src));
        const typename dst_pixfmt_t::color_type dst_color(agg::rgba(c.r, c.g, c.b, c.a));
        dst_pixfmt_t::write_plain_color(dst, dst_color);
    }
};

template<typename dst_pixfmt_t>
struct _convert_pixel<dst_pixfmt_t, agg::pixfmt_gray8>
{
    void operator()(void* dst, const void* src) const
    {
        const typename agg::pixfmt_gray8::color_type c(agg::pixfmt_gray8::read_plain_color(src));
        const typename dst_pixfmt_t::color_type dst_color(agg::rgba(c.v, c.v, c.v, c.a));
        dst_pixfmt_t::write_plain_color(dst, dst_color);
    }
};

template<typename dst_pixfmt_t>
struct _convert_pixel<dst_pixfmt_t, agg::pixfmt_sgray8>
{
    void operator()(void* dst, const void* src) const
    {
        const typename agg::pixfmt_sgray8::color_type c(agg::pixfmt_sgray8::read_plain_color(src));
        const typename dst_pixfmt_t::color_type dst_color(agg::rgba(c.v, c.v, c.v, c.a));
        dst_pixfmt_t::write_plain_color(dst, dst_color);
    }
};

template<typename dst_pixfmt_t>
struct _convert_pixel<dst_pixfmt_t, agg::pixfmt_gray16>
{
    void operator()(void* dst, const void* src) const
    {
        const typename agg::pixfmt_gray16::color_type c(agg::pixfmt_gray16::read_plain_color(src));
        const typename dst_pixfmt_t::color_type dst_color(agg::rgba(c.v, c.v, c.v, c.a));
        dst_pixfmt_t::write_plain_color(dst, dst_color);
    }
};

template<typename dst_pixfmt_t>
struct _convert_pixel<dst_pixfmt_t, agg::pixfmt_gray32>
{
    void operator()(void* dst, const void* src) const
    {
        const typename agg::pixfmt_gray32::color_type c(agg::pixfmt_gray32::read_plain_color(src));
        const typename dst_pixfmt_t::color_type dst_color(agg::rgba(c.v, c.v, c.v, c.a));
        dst_pixfmt_t::write_plain_color(dst, dst_color);
    }
};

template<typename dst_pixfmt_t, typename src_pixfmt_t>
struct _convert_row
{
    void operator()(void* dst, const void* src, unsigned width) const
    {
        _convert_pixel<dst_pixfmt_t, src_pixfmt_t> conv;
        do
        {
            conv(dst, src);
            dst = (uint8_t*)dst + dst_pixfmt_t::pix_width;
            src = (uint8_t*)src + src_pixfmt_t::pix_width;
        }
        while (--width);
    }
};

template<typename pixfmt_t>
struct _convert_row<pixfmt_t, pixfmt_t>
{
    void operator()(void* dst, const void* src, unsigned width) const
    {
        memmove(dst, src, width * pixfmt_t::pix_width);
    }
};

template<typename dst_pixfmt_t, typename src_pixfmt_t>
void _convert_buffer(agg::rendering_buffer& dst_buffer, const agg::rendering_buffer& src_buffer)
{
    agg::color_conv(&dst_buffer, &src_buffer, _convert_row<dst_pixfmt_t, src_pixfmt_t>());
}

template<typename dst_pixfmt_t>
void Image::copy_pixels(Image& dst_image) const
{
    if (get_pixel_format<dst_pixfmt_t>() != dst_image.m_format)
    {
        // XXX: scream bloody murder
    }

    agg::rendering_buffer& dst_buffer = dst_image.get_buffer();
    switch (m_format)
    {
    case k_PixelFormatGray8: _convert_buffer<dst_pixfmt_t, agg::pixfmt_gray8>(dst_buffer, m_buf);
    case k_PixelFormatSGray8: _convert_buffer<dst_pixfmt_t, agg::pixfmt_sgray8>(dst_buffer, m_buf);
    case k_PixelFormatGray16: _convert_buffer<dst_pixfmt_t, agg::pixfmt_gray16>(dst_buffer, m_buf);
    case k_PixelFormatGray32: _convert_buffer<dst_pixfmt_t, agg::pixfmt_gray32>(dst_buffer, m_buf);
    case k_PixelFormatBGR24: _convert_buffer<dst_pixfmt_t, agg::pixfmt_bgr24>(dst_buffer, m_buf);
    case k_PixelFormatRGB24: _convert_buffer<dst_pixfmt_t, agg::pixfmt_rgb24>(dst_buffer, m_buf);
    case k_PixelFormatSBGR24: _convert_buffer<dst_pixfmt_t, agg::pixfmt_sbgr24>(dst_buffer, m_buf);
    case k_PixelFormatSRGB24: _convert_buffer<dst_pixfmt_t, agg::pixfmt_srgb24>(dst_buffer, m_buf);
    case k_PixelFormatBGR48: _convert_buffer<dst_pixfmt_t, agg::pixfmt_bgr48>(dst_buffer, m_buf);
    case k_PixelFormatRGB48: _convert_buffer<dst_pixfmt_t, agg::pixfmt_rgb48>(dst_buffer, m_buf);
    case k_PixelFormatBGR96: _convert_buffer<dst_pixfmt_t, agg::pixfmt_bgr96>(dst_buffer, m_buf);
    case k_PixelFormatRGB96: _convert_buffer<dst_pixfmt_t, agg::pixfmt_rgb96>(dst_buffer, m_buf);
    case k_PixelFormatBGRA32: _convert_buffer<dst_pixfmt_t, agg::pixfmt_bgra32>(dst_buffer, m_buf);
    case k_PixelFormatRGBA32: _convert_buffer<dst_pixfmt_t, agg::pixfmt_rgba32>(dst_buffer, m_buf);
    case k_PixelFormatARGB32: _convert_buffer<dst_pixfmt_t, agg::pixfmt_argb32>(dst_buffer, m_buf);
    case k_PixelFormatABGR32: _convert_buffer<dst_pixfmt_t, agg::pixfmt_abgr32>(dst_buffer, m_buf);
    case k_PixelFormatSBGRA32: _convert_buffer<dst_pixfmt_t, agg::pixfmt_sbgra32>(dst_buffer, m_buf);
    case k_PixelFormatSRGBA32: _convert_buffer<dst_pixfmt_t, agg::pixfmt_srgba32>(dst_buffer, m_buf);
    case k_PixelFormatSARGB32: _convert_buffer<dst_pixfmt_t, agg::pixfmt_sargb32>(dst_buffer, m_buf);
    case k_PixelFormatSABGR32: _convert_buffer<dst_pixfmt_t, agg::pixfmt_sabgr32>(dst_buffer, m_buf);
    case k_PixelFormatBGRA64: _convert_buffer<dst_pixfmt_t, agg::pixfmt_bgra64>(dst_buffer, m_buf);
    case k_PixelFormatRGBA64: _convert_buffer<dst_pixfmt_t, agg::pixfmt_argb64>(dst_buffer, m_buf);
    case k_PixelFormatARGB64: _convert_buffer<dst_pixfmt_t, agg::pixfmt_argb64>(dst_buffer, m_buf);
    case k_PixelFormatABGR64: _convert_buffer<dst_pixfmt_t, agg::pixfmt_abgr64>(dst_buffer, m_buf);
    case k_PixelFormatBGRA128: _convert_buffer<dst_pixfmt_t, agg::pixfmt_bgra128>(dst_buffer, m_buf);
    case k_PixelFormatRGBA128: _convert_buffer<dst_pixfmt_t, agg::pixfmt_rgba128>(dst_buffer, m_buf);
    case k_PixelFormatARGB128: _convert_buffer<dst_pixfmt_t, agg::pixfmt_argb128>(dst_buffer, m_buf);
    case k_PixelFormatABGR128: _convert_buffer<dst_pixfmt_t, agg::pixfmt_abgr128>(dst_buffer, m_buf);
    default:
        break;
    }
}
