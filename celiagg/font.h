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

#ifndef CELIAGG_FONT_H
#define CELIAGG_FONT_H

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
#endif

class Font
{
public:

#ifdef _ENABLE_TEXT_RENDERING
#ifdef _USE_FREETYPE
    typedef agg::font_engine_freetype_int32       FontEngine;
#else
    typedef agg::font_engine_win32_tt_int32       FontEngine;
#endif
    typedef agg::font_cache_manager<FontEngine>   FontCacheManager;
#endif

    enum FontCacheType
    {
        RasterFontCache,
        VectorFontCache
    };

public:

                        Font(char const* fileName, double const height,
                             FontCacheType const ch = RasterFontCache);
                        ~Font();

    void                activate();

#ifdef _ENABLE_TEXT_RENDERING
    FontCacheManager&   cache();
#endif

    FontCacheType       cache_type() const;
    bool                flip() const;
    void                flip(bool const flip);
    double              height() const;
    bool                hinting() const;
    void                hinting(bool const hint);
    const char*         filepath() const;
    double              string_width(char const* str);
    void                transform(const agg::trans_affine& transform);

private:

    double              m_height;
    std::string         m_font_name;
    FontCacheType       m_cache_type;

#ifdef _ENABLE_TEXT_RENDERING
#ifndef _USE_FREETYPE
    thread_local static HDC                 m_font_dc;
#endif
    thread_local static FontEngine          m_font_engine;
    thread_local static FontCacheManager    m_font_cache_manager;
#endif
};

#endif // CELIAGG_FONT_H
