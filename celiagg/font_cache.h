// The MIT License (MIT)
//
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
// Author: John Wiggins

#ifndef CELIAGG_FONT_CACHE_H
#define CELIAGG_FONT_CACHE_H

#include <string>

#include <agg_basics.h>
#include <agg_font_cache_manager.h>
#include <agg_trans_affine.h>

#ifdef _ENABLE_TEXT_RENDERING
#ifdef _USE_FREETYPE
#include <agg_font_freetype.h>
#else
#include <agg_font_win32_tt.h>
#endif
#ifdef _USE_HARFBUZZ
#include "harfbuzz_shaper.h"
#else
#include "null_shaper.h"
#endif
#endif

#include "font.h"

class FontCache
{
public:

    enum GlyphType
    {
        k_GlyphTypeRaster,
        k_GlyphTypeVector
    };

#ifdef _ENABLE_TEXT_RENDERING
#ifdef _USE_FREETYPE
    typedef agg::font_engine_freetype_int32       FontEngine;
#else
    typedef agg::font_engine_win32_tt_int32       FontEngine;
#endif
    typedef agg::font_cache_manager<FontEngine>   FontCacheManager;
#ifdef _USE_HARFBUZZ
    typedef HarbuzzShaper<FontEngine>   shaper_type;
#else
    typedef NullShaper<FontEngine>      shaper_type;
#endif
#endif

                        FontCache();
                        ~FontCache();

    void                activate(const Font& font,
                                 const agg::trans_affine& transform,
                                 GlyphType const type = k_GlyphTypeRaster);
    double              measure_width(char const* str);

#ifdef _ENABLE_TEXT_RENDERING
    FontCacheManager&   manager();
    shaper_type&        shaper();
#endif

private:

#ifdef _ENABLE_TEXT_RENDERING
#ifndef _USE_FREETYPE
    HDC                 m_font_dc;  // For font_engine_win32_tt
#endif
    FontEngine          m_font_engine;
    FontCacheManager    m_font_cache_manager;
    shaper_type         m_shaper;
#endif
};

#endif // CELIAGG_FONT_CACHE_H
