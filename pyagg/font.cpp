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
#include "glyph_iter.h"

#define WIN32_FONT_WEIGHT 400


Font::Font(char const* fontName, double const height, FontCacheType const ch)
: m_cache_type(ch),
#ifdef _USE_FREETYPE
  m_font_engine(),
#else
  m_font_dc(::GetDC(0)),
  m_font_engine(m_font_dc),
#endif
  m_font_cache_manager(m_font_engine)
{
#ifdef _USE_FREETYPE
    m_font_engine.load_font(fontName,
                           0,
                           (ch == VectorFontCache) ?
                                agg::glyph_ren_outline :
                                agg::glyph_ren_agg_gray8);
    m_font_engine.hinting(true);
    m_font_engine.height(height);
#else
    m_font_engine.hinting(true);
    m_font_engine.create_font(fontName,
                             (ch == VectorFontCache) ?
                                agg::glyph_ren_outline :
                                agg::glyph_ren_agg_gray8,
                              height,
                              0.0,
                              WIN32_FONT_WEIGHT,
                              false);
#endif
}

Font::~Font()
{
#ifndef _USE_FREETYPE
    ::ReleaseDC(0, m_font_dc);
#endif

}

Font::FontCacheManager&
Font::cache()
{
    return m_font_cache_manager;
}

Font::FontCacheType
Font::cache_type() const
{
    return m_cache_type;
}

bool
Font::flip() const
{
    return m_font_engine.flip_y();
}

void
Font::flip(bool const flip)
{
    m_font_engine.flip_y(flip);
}

double
Font::height() const
{
    return m_font_engine.height();
}

bool
Font::hinting() const
{
    return m_font_engine.hinting();
}

void
Font::hinting(bool const hint)
{
    m_font_engine.hinting(hint);
}

const char*
Font::filepath() const
{
#ifdef _USE_FREETYPE
    return m_font_engine.name();
#else
    return m_font_engine.typeface();
#endif
}

double
Font::string_width(char const* str)
{
    GlyphIterator iterator(str, *this);
    while (iterator.step() != GlyphIterator::k_StepActionEnd) {}
    return iterator.x_offset();
}

void
Font::transform(const agg::trans_affine& transform)
{
    m_font_engine.transform(transform);
}
