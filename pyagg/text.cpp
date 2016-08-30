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

#include "text.h"

Font::Font(char const* fontName, double const height, bool const bold,
           bool const italic, FontCacheType const ch)
: m_cacheType(ch),
#ifdef _USE_FREETYPE
  m_fontEngine(),
#else
  m_fontDC(::GetDC(0)),
  m_fontEngine(m_fontDC),
#endif
  m_fontCacheManager(m_fontEngine)
{
#ifdef _USE_FREETYPE
    m_fontEngine.load_font(fontName,
                           0,
                           (ch == VectorFontCache) ?
                                agg::glyph_ren_outline :
                                agg::glyph_ren_agg_gray8);
    m_fontEngine.hinting(true);
    m_fontEngine.height(height);
#else
    m_fontEngine.hinting(true);
    m_fontEngine.create_font(fontName,
                             (ch == VectorFontCache) ?
                                agg::glyph_ren_outline :
                                agg::glyph_ren_agg_gray8,
                              height,
                              0.0,
                              bold ? 700 : 400,
                              italic);
#endif
    m_fontEngine.flip_y(true);
}

Font::FontCacheManager&
Font::cache()
{
    return m_fontCacheManager;
}

Font::FontCacheType
Font::cacheType() const
{
    return m_cacheType;
}

double
Font::height() const
{
    return m_fontEngine.height();
}

double
Font::stringWidth(char const* str)
{
    double x = 0;
    double y = 0;
    bool first = true;
    int index = 0;
    unsigned code = 0;

    code = getNextCodepoint(str, index);
    while (code > 0)
    {
        agg::glyph_cache const * glyph = m_fontCacheManager.glyph(code);
        if (glyph)
        {
            if (!first)
            {
                m_fontCacheManager.add_kerning(&x, &y);
            }
            x += glyph->advance_x;
            y += glyph->advance_y;
            first = false;
        }
        code = getNextCodepoint(str, index);
    }

    return x;
}

bool
Font::hinting() const
{
    return m_fontEngine.hinting();
}

void
Font::hinting(bool const hint)
{
    m_fontEngine.hinting(hint);
}

unsigned
Font::getNextCodepoint(const char *utf8, int& index)
{
    const char start = utf8[index];

    if ((start & 0x80) == 0)
    {
        // If a UCS fits 7 bits, just return it.
        return (unsigned)utf8[index++];
    }

    if ((start & 0xfc) == 0xfc)
    {
        // If a UCS fits 31 bits, it is coded as:
        // 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
        const unsigned result = (utf8[index + 0] & 0x01) << 30 |
                                (utf8[index + 1] & 0x3f) << 24 |
                                (utf8[index + 2] & 0x3f) << 18 |
                                (utf8[index + 3] & 0x3f) << 12 |
                                (utf8[index + 4] & 0x3f) << 6 |
                                (utf8[index + 5] & 0x3f);
        index += 6;
        return result;
    }
    else if ((start & 0xf8) == 0xf8)
    {
        // If a UCS fits 26 bits, it is coded as:
        // 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
        const unsigned result = (utf8[index + 0] & 0x03) << 24 |
                                (utf8[index + 1] & 0x3f) << 18 |
                                (utf8[index + 2] & 0x3f) << 12 |
                                (utf8[index + 3] & 0x3f) << 6 |
                                (utf8[index + 4] & 0x3f);
        index += 5;
        return result;
    }
    else if ((start & 0xf0) == 0xf0)
    {
        // If a UCS fits 21 bits, it is coded as:
        // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
        const unsigned result = (utf8[index + 0] & 0x07) << 18 |
                                (utf8[index + 1] & 0x3f) << 12 |
                                (utf8[index + 2] & 0x3f) << 6 |
                                (utf8[index + 3] & 0x3f);
        index += 4;
        return result;
    }
    else if ((start & 0xe0) == 0xe0)
    {
        // If a UCS fits 16 bits, it is coded as:
        // 1110xxxx 10xxxxxx 10xxxxxx
        const unsigned result = (utf8[index + 0] & 0x0f) << 12 |
                                (utf8[index + 1] & 0x3f) << 6 |
                                (utf8[index + 2] & 0x3f);
        index += 3;
        return result;
    }
    else if ((start & 0xc0) == 0xc0)
    {
        // If a UCS fits 11 bits, it is coded as:
        // 110xxxxx 10xxxxxx
        const unsigned result = (utf8[index + 0] & 0x1f) << 6 |
                                (utf8[index + 1] & 0x3f);
        index += 2;
        return result;
    }

    return 0;
}
