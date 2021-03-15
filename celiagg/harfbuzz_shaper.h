// The MIT License (MIT)
//
// Copyright (c) 2016 WUSTL ZPLAB
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
// Authors: John Wiggins

#ifndef CELIAGG_HARFBUZZ_SHAPER_H
#define CELIAGG_HARFBUZZ_SHAPER_H

#include <agg_font_cache_manager.h>
#include <hb.h>
#ifdef _USE_FREETYPE
#include <hb-ft.h>
#else
#include <hb-gdi.h>
#endif

#include "shaper.h"

// Matches the number of fonts cached by AGG
static unsigned const _max_cached_fonts = 32;

template<typename font_engine_t> class HarbuzzShaper : public Shaper
{
public:
    typedef agg::font_cache_manager<font_engine_t>   FontCacheManager;

    HarbuzzShaper(FontCacheManager& cache_manager)
    : m_cache_manager(cache_manager)
    , m_cursor_x(0.0)
    , m_cursor_y(0.0)
    , m_advance_x(0.0)
    , m_advance_y(0.0)
    , m_offset_x(0.0)
    , m_offset_y(0.0)
    , m_index(0)
    , m_buffer(hb_buffer_create())
    , m_cur_font(nullptr)
    , m_fonts(new hb_font_t* [_max_cached_fonts])
    , m_font_signatures(new char* [_max_cached_fonts])
    , m_num_fonts(0)
    {}

    ~HarbuzzShaper()
    {
        hb_buffer_destroy(m_buffer);

        for(unsigned i = 0; i < m_num_fonts; ++i)
        {
            delete [] m_font_signatures[i];
            hb_font_destroy(m_fonts[i]);
        }
        delete [] m_font_signatures;
        delete [] m_fonts;
    }

#ifdef _USE_FREETYPE
    void init_font(FT_Face face, const agg::trans_affine& transform, const char* signature)
    {
        const int idx = find_font(signature);
        if(idx >= 0)
        {
            m_cur_font = m_fonts[idx];
        }
        else
        {
            hb_font_t* font = hb_ft_font_create_referenced(face);

            add_font(font, signature);
            m_cur_font = font;
        }
        m_transform = transform;
    }
#else
    void init_font(HFONT hfont, const agg::trans_affine& transform, const char* signature)
    {
        const int idx = find_font(signature);
        if(idx >= 0)
        {
            m_cur_font = m_fonts[idx];
        }
        else
        {
            hb_face_t* tmp_face = hb_gdi_face_create(hfont);
            hb_font_t* font = hb_font_create(tmp_face);
            hb_face_destroy(tmp_face);

            add_font(font, signature);
            m_cur_font = font;
        }
        m_transform = transform;
    }
#endif

    void cursor(double const x, double const y)
    {
        m_cursor_x = x;
        m_cursor_y = y;
    }

    double cursor_x() const
    {
        return m_cursor_x + m_offset_x;
    }

    double cursor_y() const
    {
        return m_cursor_y + m_offset_y;
    }

    void shape(char const* text)
    {
        hb_buffer_reset(m_buffer);
        hb_buffer_add_utf8(m_buffer, text, -1, 0, -1);
        hb_buffer_guess_segment_properties(m_buffer);
        hb_shape(m_cur_font, m_buffer, NULL, 0);

        m_index = 0;
        m_cursor_x = m_cursor_y = 0.0;
        m_advance_x = m_advance_y = 0.0;
        m_offset_x = m_offset_y = 0.0;
    }

    StepAction step()
    {
#ifdef _ENABLE_TEXT_RENDERING
        unsigned glyph_count, index = m_index++;
        hb_glyph_info_t *glyph_info = hb_buffer_get_glyph_infos(m_buffer, &glyph_count);
        hb_glyph_position_t *glyph_pos = hb_buffer_get_glyph_positions(m_buffer, &glyph_count);

        if (index < glyph_count)
        {
            const unsigned codepoint = glyph_info[index].codepoint;
            double x_offset = glyph_pos[index].x_offset / 64.0;
            double y_offset = glyph_pos[index].y_offset / 64.0;
            double x_advance = glyph_pos[index].x_advance / 64.0;
            double y_advance = glyph_pos[index].y_advance / 64.0;
            const agg::glyph_cache* glyph = m_cache_manager.glyph(codepoint, true);
            if (glyph)
            {
                m_transform.transform_2x2(&x_offset, &y_offset);
                m_transform.transform_2x2(&x_advance, &y_advance);

                // add the previous glyph's advance
                m_cursor_x += m_advance_x;
                m_cursor_y += m_advance_y;

                // Save the offsets
                m_offset_x = x_offset;
                m_offset_y = y_offset;

                // Save for the next glyph
                m_advance_x = x_advance;
                m_advance_y = y_advance;

                return k_StepActionDraw;
            }

            return k_StepActionSkip;
        }
#endif
        return k_StepActionEnd;
    }

    //--------------------------------------------------------------------
    // Font caching code. Basically a copy of font_engine_freetype_base's code

    void add_font(hb_font_t* font, const char* signature)
    {
        // Manage a full cache
        if (m_num_fonts >= _max_cached_fonts)
        {
            delete [] m_font_signatures[0];
            hb_font_destroy(m_fonts[0]);
            memcpy(m_fonts,
                   m_fonts + 1,
                   (_max_cached_fonts - 1) * sizeof(hb_font_t*));
            memcpy(m_font_signatures,
                   m_font_signatures + 1,
                   (_max_cached_fonts - 1) * sizeof(char*));
            m_num_fonts = _max_cached_fonts - 1;
        }

        // Add the new font to the end
        m_fonts[m_num_fonts] = font;
        m_font_signatures[m_num_fonts] = new char [strlen(signature) + 1];
        strcpy(m_font_signatures[m_num_fonts], signature);
        ++m_num_fonts;
    }

    int find_font(const char* signature) const
    {
        for (unsigned i = 0; i < m_num_fonts; ++i)
        {
            if (strcmp(signature, m_font_signatures[i]) == 0) return i;
        }
        return -1;
    }

private:
    FontCacheManager&   m_cache_manager;
    agg::trans_affine   m_transform;
    double              m_cursor_x;
    double              m_cursor_y;
    double              m_advance_x;
    double              m_advance_y;
    double              m_offset_x;
    double              m_offset_y;
    unsigned            m_index;
    hb_buffer_t*        m_buffer;

    hb_font_t*          m_cur_font;
    hb_font_t**         m_fonts;
    char**              m_font_signatures;
    unsigned            m_num_fonts;
};

#endif // CELIAGG_HARFBUZZ_SHAPER_H
