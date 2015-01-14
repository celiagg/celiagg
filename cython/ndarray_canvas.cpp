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

template<>
agg::pixfmt_rgba32::color_type color_from_channel_array<agg::pixfmt_rgba32>(const uint8_t* c)
{
    return agg::pixfmt_rgba32::color_type(c[0], c[1], c[2], c[3]);
}

template<>
agg::pixfmt_rgba32::color_type color_from_channel_array<agg::pixfmt_rgb24>(const uint8_t* c)
{
    return agg::pixfmt_rgb24::color_type(c[0], c[1], c[2], c[3]);
}

template<>
agg::pixfmt_gray8::color_type color_from_channel_array<agg::pixfmt_gray8>(const uint8_t* c)
{
    return agg::pixfmt_gray8::color_type(c[0], c[1]);
}
