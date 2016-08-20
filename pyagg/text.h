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

#ifndef TEXT_H
#define TEXT_H

#include <agg_basics.h>
#include <agg_font_cache_manager.h>
#ifdef _USE_FREETYPE
#include <agg_font_freetype.h>
#else
#include <agg_font_win32_tt.h>
#endif

class Font
{
#ifdef _USE_FREETYPE
    typedef agg::font_engine_freetype_int32       FontEngine;
#else
    typedef agg::font_engine_win32_tt_int32       FontEngine;
#endif
    typedef agg::font_cache_manager<FontEngine>   FontCacheManager;

public:

    enum FontCacheType
    {
        RasterFontCache,
        VectorFontCache
    };

public:

    Font(char const* fileName, double const height,
         bool const bold = false,
         bool const italic = false,
         FontCacheType const ch = RasterFontCache);

    bool    hinting() const;
    void    hinting(bool const hint);
    double  height() const;
    double  stringWidth(char const* str);

private:

    FontCacheType       m_cacheType;
#ifndef _USE_FREETYPE
    HDC                 m_fontDC;
#endif
    FontEngine          m_fontEngine;
    FontCacheManager    m_fontCacheManager;
};

#endif // TEXT_H
