// The MIT License (MIT)
//
// Copyright (c) 2016-2021 Celiagg Contributors
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
// Author: John Wiggins

#include "font_cache.h"
#include "glyph_iter.h"

#define WIN32_FONT_WEIGHT 400

#ifndef _ENABLE_TEXT_RENDERING
// This is what happens when you disable font support!

FontCache::FontCache() {}
FontCache::~FontCache() {}
void FontCache::activate(const Font&, const agg::trans_affine& transform) {}
double FontCache::measure_width(char const* str) { return 0.0; }


#else


FontCache::FontCache()
#ifndef _USE_FREETYPE
: m_font_dc(::GetDC(0))
, m_font_engine(m_font_dc)
#else
: m_font_engine()
#endif
, m_font_cache_manager(m_font_engine)
{}

FontCache::~FontCache()
{
#ifndef _USE_FREETYPE
    ::ReleaseDC(0, m_font_dc);
#endif
}

void
FontCache::activate(const Font& font, const agg::trans_affine& transform)
{
#ifdef _USE_FREETYPE
    m_font_engine.load_font(font.filepath(),
                            font.face_index(),
                            (font.cache_type() == Font::VectorFontCache) ?
                                agg::glyph_ren_outline :
                                agg::glyph_ren_agg_gray8);
    // Manipulate the aspects of the font which was just loaded.
    // XXX: If this is done before, draw_text draws nothing on its first call.
    m_font_engine.flip_y(font.flip());
    m_font_engine.hinting(font.hinting());
    m_font_engine.height(font.height());
    m_font_engine.transform(transform);
#else
    // Set the font aspects _before_ calling create_font to work around the
    // Windows font engine's lack of cache signature updating.
    m_font_engine.flip_y(font.flip());
    m_font_engine.hinting(font.hinting());
    m_font_engine.transform(transform);
    m_font_engine.create_font(font.filepath(),
                             (font.cache_type() == Font::VectorFontCache) ?
                                agg::glyph_ren_outline :
                                agg::glyph_ren_agg_gray8,
                              font.height(),
                              0.0,
                              WIN32_FONT_WEIGHT,
                              false);
#endif
}

double
FontCache::measure_width(char const* str)
{
    GlyphIterator iterator(str, *this);
    while (iterator.step() != GlyphIterator::k_StepActionEnd) {}
    return iterator.x_offset();
}

FontCache::FontCacheManager&
FontCache::manager()
{
    return m_font_cache_manager;
}

#endif
