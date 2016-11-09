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

Image::Image(uint8_t* buf, unsigned width, unsigned height, int stride)
: m_buf(buf, width, height, stride)
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
