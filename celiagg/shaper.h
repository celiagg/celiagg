// The MIT License (MIT)
//
// Copyright (c) 2016 WUSTL ZPLAB
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
// Authors: John Wiggins

#ifndef CELIAGG_SHAPER_H
#define CELIAGG_SHAPER_H

#include <agg_trans_affine.h>

class Shaper
{
public:
    enum StepAction
    {
        k_StepActionDraw = 0,
        k_StepActionSkip,
        k_StepActionEnd,
    };

#ifdef _USE_FREETYPE
    void init_font(FT_Face face, const agg::trans_affine& transform, const char* signature) {}
#else
    void init_font(HFONT hfont, const agg::trans_affine& transform, const char* signature) {}
#endif

    void cursor(double const x, double const y) {}
    double offset_x() const { return 0.0; }
    double offset_y() const { return 0.0; }

    void shape(char const* text) {}
    StepAction step() { return k_StepActionEnd; }
};

#endif // CELIAGG_SHAPER_H
