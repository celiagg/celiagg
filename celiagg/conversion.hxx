// The MIT License (MIT)
//
// Copyright (c) 2017-2023 Celiagg Contributors
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

template <typename T> T _get_max_value();
template <> unsigned char _get_max_value() { return 255; }
template <> unsigned short _get_max_value() { return 65535; }
template <> float _get_max_value() { return 1.0; }

template <class src_t, class dst_t>
void gray2rgb(const src_t* src, dst_t* dst, const size_t count,
    const int dst_order[4], const bool dst_alpha)
{
    const src_t max_from = _get_max_value<src_t>();
    const dst_t max_to = _get_max_value<dst_t>();
    const dst_t scale = (dst_t)(max_to / (float)max_from);

    for (size_t i=0; i < count; ++i)
    {
        dst[i + dst_order[0]] = (dst_t)(src[i] * scale);
        dst[i + dst_order[1]] = (dst_t)(src[i] * scale);
        dst[i + dst_order[2]] = (dst_t)(src[i] * scale);
        if (dst_alpha) dst[i + dst_order[3]] = max_to;
    }
}

template <class src_t, class dst_t>
void rgb2rgb(const src_t* src, dst_t* dst, const size_t count,
    const int src_order[4], const int dst_order[4],
    const bool src_alpha, const bool dst_alpha)
{
    const src_t max_from = _get_max_value<src_t>();
    const dst_t max_to = _get_max_value<dst_t>();
    const dst_t scale = (dst_t)(max_to / (float)max_from);

    for (size_t i=0; i < count; ++i)
    {
        dst[i + dst_order[0]] = (dst_t)(src[i + src_order[0]] * scale);
        dst[i + dst_order[1]] = (dst_t)(src[i + src_order[1]] * scale);
        dst[i + dst_order[2]] = (dst_t)(src[i + src_order[2]] * scale);
        if (dst_alpha)
        {
            if (src_alpha) dst[i + dst_order[3]] = src[i + src_order[3]] * scale;
            else dst[i + dst_order[3]] = max_to;
        }
    }
}

template <class src_t, class dst_t>
void gray2gray(const src_t* src, dst_t* dst, const size_t count)
{
    const src_t max_from = _get_max_value<src_t>();
    const dst_t max_to = _get_max_value<dst_t>();
    const dst_t scale = (dst_t)(max_to / (float)max_from);

    for (size_t i=0; i < count; ++i)
    {
        dst[i] = (dst_t)(src[i] * scale);
    }
}

template <class src_t, class dst_t>
void rgb2gray(const src_t* src, dst_t* dst, const size_t count, const int src_order[4])
{
    const src_t max_from = _get_max_value<src_t>();
    const dst_t max_to = _get_max_value<dst_t>();
    const dst_t scale = (dst_t)(max_to / (3.0 * max_from));

    for (size_t i=0; i < count; ++i)
    {
        dst[i] = (dst_t)((src[i + src_order[0]] +
                          src[i + src_order[1]] +
                          src[i + src_order[2]]) * scale);
    }
}
