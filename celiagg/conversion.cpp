// The MIT License (MIT)
//
// Copyright (c) 2017-2023 Celiagg Contributors
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

#include "conversion.h"
#include "conversion.hxx"

void convert_pixels(const unsigned char* src, unsigned char* dst,
    const size_t count, const PixelFormat src_format, const PixelFormat dst_format)
{
    const int BGRA_ORDER[] = {2, 1, 0, 3};
    const int RGBA_ORDER[] = {0, 1, 2, 3};
    const int RGB_ORDER[] = {0, 1, 2, 0};

    switch (src_format)
    {
        case k_PixelFormatRGBA128:
        {
            switch (dst_format)
            {
                case k_PixelFormatBGRA32:
                    rgb2rgb(reinterpret_cast<const float*>(src), dst, count,
                            RGBA_ORDER, BGRA_ORDER, true, true);
                    break;
                case k_PixelFormatRGBA32:
                    rgb2rgb(reinterpret_cast<const float*>(src), dst, count,
                            RGBA_ORDER, RGBA_ORDER, true, true);
                    break;
                case k_PixelFormatRGB24:
                    rgb2rgb(reinterpret_cast<const float*>(src), dst, count,
                            RGBA_ORDER, RGB_ORDER, true, false);
                    break;
                case k_PixelFormatGray8:
                    rgb2gray(reinterpret_cast<const float*>(src), dst, count,
                             RGBA_ORDER);
                    break;
                default:
                    break;
            }
            break;
        }
        case k_PixelFormatBGRA32:
        {
            switch (dst_format)
            {
                case k_PixelFormatRGBA128:
                    rgb2rgb(src, reinterpret_cast<float*>(dst), count,
                            RGBA_ORDER, RGBA_ORDER, true, true);
                    break;
                case k_PixelFormatRGBA32:
                    rgb2rgb(src, dst, count, RGBA_ORDER, RGBA_ORDER, true, true);
                    break;
                case k_PixelFormatRGB24:
                    rgb2rgb(src, dst, count, RGBA_ORDER, RGB_ORDER, true, false);
                    break;
                case k_PixelFormatGray8:
                    rgb2gray(src, dst, count, RGBA_ORDER);
                    break;
                default:
                    break;
            }
            break;
        }
        case k_PixelFormatRGBA32:
        {
            switch (dst_format)
            {
                case k_PixelFormatRGBA128:
                    rgb2rgb(src, reinterpret_cast<float*>(dst), count,
                            RGBA_ORDER, RGBA_ORDER, true, true);
                    break;
                case k_PixelFormatBGRA32:
                    rgb2rgb(src, dst, count, RGBA_ORDER, BGRA_ORDER, true, true);
                    break;
                case k_PixelFormatRGB24:
                    rgb2rgb(src, dst, count, RGBA_ORDER, RGB_ORDER, true, false);
                    break;
                case k_PixelFormatGray8:
                    rgb2gray(src, dst, count, RGBA_ORDER);
                    break;
                default:
                    break;
            }
            break;
        }
        case k_PixelFormatRGB24:
        {
            switch (dst_format)
            {
                case k_PixelFormatRGBA128:
                    rgb2rgb(src, reinterpret_cast<float*>(dst), count,
                            RGB_ORDER, RGBA_ORDER, false, true);
                    break;
                case k_PixelFormatBGRA32:
                    rgb2rgb(src, dst, count, RGB_ORDER, BGRA_ORDER, false, true);
                    break;
                case k_PixelFormatRGBA32:
                    rgb2rgb(src, dst, count, RGB_ORDER, RGBA_ORDER, false, true);
                    break;
                case k_PixelFormatGray8:
                    rgb2gray(src, dst, count, RGB_ORDER);
                    break;
                default:
                    break;
            }
            break;
        }
        case k_PixelFormatGray8:
        {
            switch (dst_format)
            {
                case k_PixelFormatRGBA128:
                    gray2rgb(src, reinterpret_cast<float*>(dst), count,
                             RGBA_ORDER, true);
                    break;
                case k_PixelFormatBGRA32:
                    gray2rgb(src, dst, count, BGRA_ORDER, true);
                    break;
                case k_PixelFormatRGBA32:
                    gray2rgb(src, dst, count, RGBA_ORDER, true);
                    break;
                case k_PixelFormatRGB24:
                    gray2rgb(src, dst, count, RGB_ORDER, false);
                    break;
                default:
                    break;
            }
            break;
        }
        default:
            break;
    }
}
