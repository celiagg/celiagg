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

#ifndef PYAGG_GLYPH_ITER_H
#define PYAGG_GLYPH_ITER_H

#include "font.h"

class GlyphIterator
{
public:
    enum StepAction
    {
        k_StepActionInvalid = -1,
        k_StepActionDraw = 0,
        k_StepActionSkip,
        k_StepActionEnd,
        k_StepActionCount
    };

public:
                        GlyphIterator(char const* utf8Text, Font& font,
                                      const bool drawing = false,
                                      const double x_Offset = 0.0,
                                      const double y_Offset = 0.0);

    StepAction          step();
    double              xOffset() const { return m_offset_x; }
    double              yOffset() const { return m_offset_y; }

private:
    unsigned            _getNextCodepoint();

private:
    Font&       m_font;
    double      m_offset_x;
    double      m_offset_y;
    char const* m_text;
    int         m_index; 
    bool        m_is_drawing;
};

#endif // PYAGG_GLYPH_ITER_H
