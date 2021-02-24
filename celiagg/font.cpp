// The MIT License (MIT)
//
// Copyright (c) 2016 WUSTL ZPLAB
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
// Authors: John Wiggins

#include "font.h"

Font::Font(char const* fontName, double const height, FontCacheType const ch, unsigned const face_index)
: m_height(height)
, m_font_name(fontName)
, m_cache_type(ch)
, m_face_index(face_index)
, m_flip(false)
, m_hinting(true)
{}

Font::FontCacheType
Font::cache_type() const
{
    return m_cache_type;
}

unsigned
Font::face_index() const
{
    return m_face_index;
}

const char*
Font::filepath() const
{
    return m_font_name.c_str();
}

bool
Font::flip() const
{
    return m_flip;
}

void
Font::flip(bool const flip)
{
    m_flip = flip;
}

double
Font::height() const
{
    return m_height;
}

void
Font::height(double const height)
{
    m_height = height;
}

bool
Font::hinting() const
{
    return m_hinting;
}

void
Font::hinting(bool const hint)
{
    m_hinting = hint;
}
