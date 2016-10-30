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

#include "glyph_iter.h"

GlyphIterator::GlyphIterator(char const* utf8Text, Font& font, const bool drawing,
                             const double x_off, const double y_off)
: m_font(font)
, m_offset_x(x_off)
, m_offset_y(y_off)
, m_text(utf8Text)
, m_index(0)
, m_is_drawing(drawing)
{}

GlyphIterator::StepAction
GlyphIterator::step()
{
    const unsigned codepoint = _get_next_codepoint();
    if (codepoint != 0)
    {
        const agg::glyph_cache* glyph = m_font.cache().glyph(codepoint);
        if (glyph)
        {
            if (m_index > 0)
            {
                m_font.cache().add_kerning(&m_offset_x, &m_offset_y);
            }
            if (m_is_drawing)
            {
                m_font.cache().init_embedded_adaptors(glyph, m_offset_x, m_offset_y);
            }
            m_offset_x += glyph->advance_x;
            m_offset_y += glyph->advance_y;

            return k_StepActionDraw;
        }

        return k_StepActionSkip;
    }

    return k_StepActionEnd;
}

unsigned
GlyphIterator::_get_next_codepoint()
{
    const char start = m_text[m_index];

    if ((start & 0x80) == 0)
    {
        // If a UCS fits 7 bits, just return it.
        return (unsigned)m_text[m_index++];
    }

    if ((start & 0xfc) == 0xfc)
    {
        // If a UCS fits 31 bits, it is coded as:
        // 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
        const unsigned result = (m_text[m_index + 0] & 0x01) << 30 |
                                (m_text[m_index + 1] & 0x3f) << 24 |
                                (m_text[m_index + 2] & 0x3f) << 18 |
                                (m_text[m_index + 3] & 0x3f) << 12 |
                                (m_text[m_index + 4] & 0x3f) << 6 |
                                (m_text[m_index + 5] & 0x3f);
        m_index += 6;
        return result;
    }
    else if ((start & 0xf8) == 0xf8)
    {
        // If a UCS fits 26 bits, it is coded as:
        // 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
        const unsigned result = (m_text[m_index + 0] & 0x03) << 24 |
                                (m_text[m_index + 1] & 0x3f) << 18 |
                                (m_text[m_index + 2] & 0x3f) << 12 |
                                (m_text[m_index + 3] & 0x3f) << 6 |
                                (m_text[m_index + 4] & 0x3f);
        m_index += 5;
        return result;
    }
    else if ((start & 0xf0) == 0xf0)
    {
        // If a UCS fits 21 bits, it is coded as:
        // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
        const unsigned result = (m_text[m_index + 0] & 0x07) << 18 |
                                (m_text[m_index + 1] & 0x3f) << 12 |
                                (m_text[m_index + 2] & 0x3f) << 6 |
                                (m_text[m_index + 3] & 0x3f);
        m_index += 4;
        return result;
    }
    else if ((start & 0xe0) == 0xe0)
    {
        // If a UCS fits 16 bits, it is coded as:
        // 1110xxxx 10xxxxxx 10xxxxxx
        const unsigned result = (m_text[m_index + 0] & 0x0f) << 12 |
                                (m_text[m_index + 1] & 0x3f) << 6 |
                                (m_text[m_index + 2] & 0x3f);
        m_index += 3;
        return result;
    }
    else if ((start & 0xc0) == 0xc0)
    {
        // If a UCS fits 11 bits, it is coded as:
        // 110xxxxx 10xxxxxx
        const unsigned result = (m_text[m_index + 0] & 0x1f) << 6 |
                                (m_text[m_index + 1] & 0x3f);
        m_index += 2;
        return result;
    }

    return 0;
}
