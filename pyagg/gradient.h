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

#ifndef GRADIENT_H
#define GRADIENT_H

#include "agg_basics.h"
#include "agg_trans_affine.h"
#include "agg_span_gradient.h"


template <typename ColorT, int FUNC_SIZE = 256>
class Gradient 
{
public:
    virtual ~Gradient() {}

protected:
    typedef agg::pod_auto_array<ColorT, FUNC_SIZE> GradientArray;

    GradientArray       m_Func;
    agg::trans_affine   m_Transform;
    double              m_D1;
    double              m_D2;
};


template <typename ColorT, int FUNC_SIZE = 256>
class LinearGradient : public Gradient<ColorT, FUNC_SIZE>
{
public:
    LinearGradient(double const x1, double const y1,
                   double const x2, double const y2,
                   ColorT const & c1, ColorT const & c2, double const profile)
    {
        int const kHalfFunc = FUNC_SIZE/2;
        int const startGradient = kHalfFunc - int(profile * double(kHalfFunc - 1));
        int endGradient = kHalfFunc + int(profile * double(kHalfFunc - 1));

        if (endGradient <= startGradient)
        {
            endGradient = startGradient + 1;
        } 

        int i;
        double const k = 1.0 / double(endGradient - startGradient);
        for (i = 0; i < startGradient; i++)
        {
            this->m_Func[i] = c1;
        }
        for (; i < endGradient; i++)
        {
            this->m_Func[i] = c1.gradient(c2, double(i - startGradient) * k);
        }
        for (; i < FUNC_SIZE; i++)
        {
            this->m_Func[i] = c2;
        }

        double const angle = atan2(y2-y1, x2-x1);
        this->m_Transform *= agg::trans_affine_rotation(angle);
        this->m_Transform *= agg::trans_affine_translation(x1, y1);
        this->m_D1 = 0.0;
        this->m_D2 = sqrt((x2-x1) * (x2-x1) + (y2-y1) * (y2-y1));
    }
};


template <typename ColorT, int FUNC_SIZE = 256>
class RadialGradient : public Gradient<ColorT, FUNC_SIZE>
{
public:
    RadialGradient(double const x, double const y, double const r,
                   ColorT const & c1, ColorT const & c2,
                   double const profile)
    {
        int const kHalfFunc = FUNC_SIZE/2;
        int const startGradient = kHalfFunc - int(profile * double(kHalfFunc - 1));
        int endGradient = kHalfFunc + int(profile * double(kHalfFunc - 1));

        if (endGradient <= startGradient)
        {
            endGradient = startGradient + 1;
        }

        int i;
        double const k = 1.0 / double(endGradient - startGradient);
        for (i = 0; i < startGradient; i++)
        {
            this->m_Func[i] = c1;
        }
        for (; i < endGradient; i++)
        {
            this->m_Func[i] = c1.gradient(c2, double(i - startGradient) * k);
        }
        for (; i < 256; i++)
        {
            this->m_Func[i] = c2;
        }
        this->m_Transform *= agg::trans_affine_translation(x, y);
        this->m_D1 = 0;
        this->m_D1 = r;
    }
};

typedef LinearGradient<agg::rgba8, 256> LinearGradientByte;
typedef LinearGradient<agg::rgba32, 256> LinearGradientFloat;
typedef RadialGradient<agg::rgba8, 256> RadialGradientByte;
typedef RadialGradient<agg::rgba32, 256> RadialGradientFloat;

#endif // GRADIENT_H