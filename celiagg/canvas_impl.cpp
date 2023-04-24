// The MIT License (MIT)
//
// Copyright (c) 2014-2015 WUSTL ZPLAB
// Copyright (c) 2016-2023 Celiagg Contributors
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
//          John Wiggins

#include "canvas.h"
#include "canvas_impl.h"

typedef canvas<agg::pixfmt_rgba128> canvas_rgba128_t;
typedef canvas<agg::pixfmt_bgra32> canvas_bgra32_t;
typedef canvas<agg::pixfmt_rgba32> canvas_rgba32_t;
typedef canvas<agg::pixfmt_rgb24> canvas_rgb24_t;
typedef canvas<agg::pixfmt_gray8> canvas_ga16_t;


canvas_base* canvas_rgba128(unsigned char* buf,
    const unsigned width, const unsigned height, const int stride,
    FontCache& cache, const bool bottom_up)
{
    return static_cast<canvas_base*>(
        new canvas_rgba128_t(buf, width, height, stride, 4, cache, bottom_up)
    );
}

canvas_base* canvas_bgra32(unsigned char* buf,
    const unsigned width, const unsigned height, const int stride,
    FontCache& cache, const bool bottom_up)
{
    return static_cast<canvas_base*>(
        new canvas_bgra32_t(buf, width, height, stride, 4, cache, bottom_up)
    );
}

canvas_base* canvas_rgba32(unsigned char* buf,
    const unsigned width, const unsigned height, const int stride,
    FontCache& cache, const bool bottom_up)
{
    return static_cast<canvas_base*>(
        new canvas_rgba32_t(buf, width, height, stride, 4, cache, bottom_up)
    );
}

canvas_base* canvas_rgb24(unsigned char* buf,
    const unsigned width, const unsigned height, const int stride,
    FontCache& cache, const bool bottom_up)
{
    return static_cast<canvas_base*>(
        new canvas_rgb24_t(buf, width, height, stride, 3, cache, bottom_up)
    );
}

canvas_base* canvas_ga16(unsigned char* buf,
    const unsigned width, const unsigned height, const int stride,
    FontCache& cache, const bool bottom_up)
{
    return static_cast<canvas_base*>(
        new canvas_ga16_t(buf, width, height, stride, 2, cache, bottom_up)
    );
}

canvas_base* canvas_g8(unsigned char* buf,
    const unsigned width, const unsigned height, const int stride,
    FontCache& cache, const bool bottom_up)
{
    return static_cast<canvas_base*>(
        new canvas_ga16_t(buf, width, height, stride, 1, cache, bottom_up)
    );
}
