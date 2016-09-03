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


template <typename pixfmt_t, typename rasterizer_t, typename renderer_t>
void Paint::render(rasterizer_t& ras, renderer_t& renderer)
{
    switch (m_type)
    {
    case Paint::k_PaintTypeSolid:
        _render_solid<pixfmt_t, rasterizer_t, renderer_t>(ras, renderer);
        break;
    case Paint::k_PaintTypeLinearGradient:
        _render_linear_grad<pixfmt_t, rasterizer_t, renderer_t>(ras, renderer);
        break;
    case Paint::k_PaintTypeRadialGradient:
        _render_radial_grad<pixfmt_t, rasterizer_t, renderer_t>(ras, renderer);
        break;
    default:
        break;
    }
}

template <typename pixfmt_t, typename array_t>
void Paint::_generate_colors(array_t& array)
{
    double offset = 0.0;
    unsigned arr_index = 0;
    const unsigned int array_size = array.size();

    for (unsigned stop_idx = 0; stop_idx < m_stops.size()-1; ++stop_idx)
    {
        const GradientStop& curr_stop = m_stops[stop_idx];
        const GradientStop& next_stop = m_stops[stop_idx+1];
        const agg::rgba curr_rgba(curr_stop.r, curr_stop.g, curr_stop.b, curr_stop.a);
        const agg::rgba next_rgba(next_stop.r, next_stop.g, next_stop.b, next_stop.a);
        const typename pixfmt_t::color_type curr_color = typename pixfmt_t::color_type(curr_rgba);
        const typename pixfmt_t::color_type next_color = typename pixfmt_t::color_type(next_rgba);
        const double offset_range = next_stop.off - curr_stop.off;

        while (offset <= next_stop.off && arr_index < array_size)
        {
            array[arr_index++] = curr_color.gradient(next_color, (offset - curr_stop.off) / offset_range);
            offset = arr_index / double(array_size - 1);
        }
    }
}

template <typename pixfmt_t, typename rasterizer_t, typename renderer_t>
void Paint::_render_solid(rasterizer_t& ras, renderer_t& renderer)
{
    typedef agg::renderer_scanline_aa_solid<renderer_t> solid_renderer_t;

    solid_renderer_t solid_renderer(renderer);
    agg::scanline_u8 scanline;
    typename pixfmt_t::color_type color(m_color);

    solid_renderer.color(color);
    agg::render_scanlines(ras, scanline, solid_renderer);
}

template <typename pixfmt_t, typename rasterizer_t, typename renderer_t>
void Paint::_render_linear_grad(rasterizer_t& ras, renderer_t& renderer)
{
    if (m_points[0] == m_points[2])
    {
        typedef agg::gradient_y function_t; 
        function_t func;
        _render_spread_grad<pixfmt_t, rasterizer_t, renderer_t, function_t>(ras, renderer, func);
    }
    else if (m_points[1] == m_points[3])
    {
        typedef agg::gradient_x function_t;
        function_t func;
        _render_spread_grad<pixfmt_t, rasterizer_t, renderer_t, function_t>(ras, renderer, func);
    }
    else
    {
        typedef agg::gradient_x function_t;
        function_t func;
        _render_spread_grad<pixfmt_t, rasterizer_t, renderer_t, function_t>(ras, renderer, func);
    }
}

template <typename pixfmt_t, typename rasterizer_t, typename renderer_t>
void Paint::_render_radial_grad(rasterizer_t& ras, renderer_t& renderer)
{
    typedef agg::gradient_radial_focus function_t;
    function_t func(m_points[2], m_points[4] - m_points[0], m_points[5] - m_points[1]);
    _render_spread_grad<pixfmt_t, rasterizer_t, renderer_t, function_t>(ras, renderer, func);
}

template <typename pixfmt_t, typename rasterizer_t, typename renderer_t, typename grad_func_t>
void Paint::_render_spread_grad(rasterizer_t& ras, renderer_t& renderer, grad_func_t& func)
{
    // apply the proper fill adapter based on the spread method
    switch (m_spread)
    {
        case Paint::k_GradientSpreadReflect:
        {
            typedef agg::gradient_reflect_adaptor<grad_func_t> adaptor_t; 
            adaptor_t adaptor(func);
            _render_final_grad<pixfmt_t, rasterizer_t, renderer_t, adaptor_t>(ras, renderer, adaptor);
            break;
        }
        case Paint::k_GradientSpreadRepeat:
        {
            typedef agg::gradient_repeat_adaptor<grad_func_t> adaptor_t;
            adaptor_t adaptor(func);
            _render_final_grad<pixfmt_t, rasterizer_t, renderer_t, adaptor_t>(ras, renderer, adaptor);
            break;
        }
        default:
        {
            _render_final_grad<pixfmt_t, rasterizer_t, renderer_t, grad_func_t>(ras, renderer, func);
            break;
        }
    }
}

template <typename pixfmt_t, typename rasterizer_t, typename renderer_t, typename grad_func_t>
void Paint::_render_final_grad(rasterizer_t& ras, renderer_t& renderer, grad_func_t& func)
{
    typedef agg::span_interpolator_linear<> interpolator_t;
    typedef agg::pod_auto_array<typename pixfmt_t::color_type, 256> color_array_t;
    typedef agg::span_gradient<typename pixfmt_t::color_type, interpolator_t, grad_func_t, color_array_t> span_gradient_t;
    typedef agg::span_allocator<typename pixfmt_t::color_type> span_allocator_t;
    typedef agg::renderer_scanline_aa<renderer_t, span_allocator_t, span_gradient_t> gradient_renderer_t;

    agg::trans_affine gradient_mtx;
    interpolator_t span_interpolator(gradient_mtx);
    span_allocator_t span_allocator;
    color_array_t color_array;
    agg::scanline_u8 scanline;

    double d1 = 0, d2 = 0;
    if (m_type == Paint::k_PaintTypeRadialGradient && m_points.size() > 4)
    {
        // length is the radius
        d2 = m_points[2];
    }
    else if (m_type == Paint::k_PaintTypeLinearGradient)
    {
        // length is the distance between the two points
        const double dx = m_points[2] - m_points[0];
        const double dy = m_points[3] - m_points[1];
        d2 = sqrt(dx * dx + dy * dy);
        
        if (m_points[0] == m_points[2])
        {
            // gradient_y. handle flips
            gradient_mtx *= agg::trans_affine_rotation(atan2(0.0, dy));
        }
        else if (m_points[1] == m_points[3])
        {
            // gradient_x. handle flips
            gradient_mtx *= agg::trans_affine_rotation(atan2(0.0, dx));
        }
        else
        {
            // general case: arbitrary rotation
            gradient_mtx *= agg::trans_affine_rotation(atan2(dy, dx));
        }
    }

    gradient_mtx *= agg::trans_affine_translation(m_points[0], m_points[1]);
    if (m_units == Paint::k_GradientUnitsUserSpace)
    {
        gradient_mtx *= m_transform;
    }
    gradient_mtx.invert();

    _generate_colors<pixfmt_t, color_array_t>(color_array);

    span_gradient_t span_gradient(span_interpolator, func, color_array, d1, d2);
    gradient_renderer_t grad_renderer(renderer, span_allocator, span_gradient);
    agg::render_scanlines(ras, scanline, grad_renderer);
}
