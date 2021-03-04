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

#ifndef CELIAGG_FONT_H
#define CELIAGG_FONT_H

#include <string>

class Font
{
public:
    // These map directly to the weights used by the WinGDI CreateFont function
    enum FontWeight {
        k_FontWeightAny = 0,
        k_FontWeightThin = 100,
        k_FontWeightExtraLight = 200,
        k_FontWeightLight = 300,
        k_FontWeightRegular = 400,
        k_FontWeightMedium = 500,
        k_FontWeightSemiBold = 600,
        k_FontWeightBold = 700,
        k_FontWeightExtraBold = 800,
        k_FontWeightHeavy = 900,
    };

public:

                    Font(char const* face_or_path, double const height,
                         unsigned const face_index = 0,
                         FontWeight const weight = k_FontWeightRegular,
                         bool const italic = false);

    unsigned        face_index() const;
    const char*     face_or_path() const;

    bool            flip() const;
    void            flip(bool const flip);

    double          height() const;
    void            height(double const height);

    bool            hinting() const;
    void            hinting(bool const hint);

    bool            italic() const;
    void            italic(bool const italic);

    FontWeight      weight() const;
    void            weight(FontWeight const weight);

private:

    std::string     m_face_or_path;
    double          m_height;
    FontWeight      m_weight;
    unsigned        m_face_index;
    bool            m_flip;
    bool            m_hinting;
    bool            m_italic;
};

#endif // CELIAGG_FONT_H
