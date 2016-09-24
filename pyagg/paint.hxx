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

template <typename pixfmt_t, typename array_t>
void _generate_colors(const agg::pod_array_adaptor<GradientStop>& stops, array_t& array)
{
    double offset = 0.0;
    unsigned arr_index = 0;
    const unsigned int array_size = array.size();

    for (unsigned stop_idx = 0; stop_idx < stops.size()-1; ++stop_idx)
    {
        const GradientStop& curr_stop = stops[stop_idx];
        const GradientStop& next_stop = stops[stop_idx+1];
        const agg::rgba curr_rgba(curr_stop.r, curr_stop.g, curr_stop.b, curr_stop.a);
        const agg::rgba next_rgba(next_stop.r, next_stop.g, next_stop.b, next_stop.a);
        const typename pixfmt_t::color_type curr_color(curr_rgba);
        const typename pixfmt_t::color_type next_color(next_rgba);
        const double offset_range = next_stop.off - curr_stop.off;

        while (offset <= next_stop.off && arr_index < array_size)
        {
            array[arr_index++] = curr_color.gradient(next_color, (offset - curr_stop.off) / offset_range);
            offset = arr_index / double(array_size - 1);
        }
    }
}

template <typename rasterizer_t>
void _rasterizer_path_bbox(rasterizer_t& ras, double& x, double& y, double& w, double& h)
{
    x = ras.min_x();
    y = ras.min_y();
    w = ras.max_x() - w;
    h = ras.max_y() - y;
}

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

    case Paint::k_PaintTypePattern:
        _render_pattern<pixfmt_t, rasterizer_t, renderer_t>(ras, renderer);
        break;

    default:
        break;
    }
}


template <typename pixfmt_t, typename rasterizer_t, typename renderer_t>
void Paint::_render_linear_grad(rasterizer_t& ras, renderer_t& renderer)
{
    typedef agg::pod_auto_vector<double, k_LinearPointsSize> vector_t;

    // Copy the points
    vector_t points;
    for (int i=0; i < k_LinearPointsSize; ++i)
        points.add(m_points[i]);

    if (m_units == Paint::k_GradientUnitsObjectBoundingBox)
    {
        double x, y, w, h;
        _rasterizer_path_bbox(ras, x, y, w, h);
        points[k_LinearX1] = x + points[k_LinearX1] * w;
        points[k_LinearX2] = x + points[k_LinearX2] * w;
        points[k_LinearY1] = y + points[k_LinearY1] * h;
        points[k_LinearY2] = y + points[k_LinearY2] * h;
    }

    if (points[0] == points[2])
    {
        typedef agg::gradient_y function_t;
        function_t func;
        _render_spread_grad<pixfmt_t, rasterizer_t, renderer_t, function_t, vector_t>(ras, renderer, func, points);
    }
    else if (points[1] == points[3])
    {
        typedef agg::gradient_x function_t;
        function_t func;
        _render_spread_grad<pixfmt_t, rasterizer_t, renderer_t, function_t, vector_t>(ras, renderer, func, points);
    }
    else
    {
        typedef agg::gradient_x function_t;
        function_t func;
        _render_spread_grad<pixfmt_t, rasterizer_t, renderer_t, function_t, vector_t>(ras, renderer, func, points);
    }
}

template <typename pixfmt_t, typename rasterizer_t, typename renderer_t>
void Paint::_render_radial_grad(rasterizer_t& ras, renderer_t& renderer)
{
    // m_points: cx, cy, r, fx, fy
    typedef agg::pod_auto_vector<double, k_RadialPointsSize> vector_t;

    // Copy the points
    vector_t points;
    for (int i=0; i < k_RadialPointsSize; ++i)
        points.add(m_points[i]);

    if (m_units == Paint::k_GradientUnitsObjectBoundingBox)
    {
        double x, y, w, h;
        _rasterizer_path_bbox(ras, x, y, w, h);
        points[k_RadialR] = points[k_RadialR] * w;
        points[k_RadialCX] = x + points[k_RadialCX] * w;
        points[k_RadialFX] = x + points[k_RadialFX] * w;
        points[k_RadialCY] = y + points[k_RadialCY] * h;
        points[k_RadialFY] = y + points[k_RadialFY] * h;
    }

    // function args: radius, focus dx, focus dy
    typedef agg::gradient_radial_focus grad_func_t;
    agg::gradient_radial_focus func(points[k_RadialR],
                                    points[k_RadialFX] - points[k_RadialCX],
                                    points[k_RadialFY] - points[k_RadialCY]);
    _render_spread_grad<pixfmt_t, rasterizer_t, renderer_t, grad_func_t, vector_t>(ras, renderer, func, points);
}

template <typename pixfmt_t, typename rasterizer_t, typename renderer_t, typename grad_func_t, typename vector_t>
void Paint::_render_spread_grad(rasterizer_t& ras, renderer_t& renderer, grad_func_t& func, vector_t& points)
{
    // apply the proper fill adapter based on the spread method
    switch (m_spread)
    {
    case Paint::k_GradientSpreadReflect:
        {
            typedef agg::gradient_reflect_adaptor<grad_func_t> adapted_func_t;
            agg::gradient_reflect_adaptor<grad_func_t> adaptor(func);
            _render_gradient_final<pixfmt_t, rasterizer_t, renderer_t, adapted_func_t, vector_t>(ras, renderer, adaptor, points);
        }
        break;

    case Paint::k_GradientSpreadRepeat:
        {
            typedef agg::gradient_repeat_adaptor<grad_func_t> adapted_func_t;
            agg::gradient_repeat_adaptor<grad_func_t> adaptor(func);
            _render_gradient_final<pixfmt_t, rasterizer_t, renderer_t, adapted_func_t, vector_t>(ras, renderer, adaptor, points);
        }
        break;

    case Paint::k_GradientSpreadPad:
    default:
        _render_gradient_final<pixfmt_t, rasterizer_t, renderer_t, grad_func_t, vector_t>(ras, renderer, func, points);
        break;
    }
}

template <typename pixfmt_t, typename rasterizer_t, typename renderer_t, typename grad_func_t, typename vector_t>
void Paint::_render_gradient_final(rasterizer_t& ras, renderer_t& renderer, grad_func_t& func, vector_t& points)
{
    typedef agg::span_interpolator_linear<> span_interpolator_t;
    typedef agg::pod_auto_array<typename pixfmt_t::color_type, 256> color_array_t;
    typedef agg::span_gradient<typename pixfmt_t::color_type, span_interpolator_t, grad_func_t, color_array_t> span_gradient_t;
    typedef agg::span_allocator<typename pixfmt_t::color_type> span_allocator_t;
    typedef agg::renderer_scanline_aa<renderer_t, span_allocator_t, span_gradient_t> gradient_renderer_t;

    agg::trans_affine gradient_mtx;
    span_interpolator_t span_interpolator(gradient_mtx);
    span_allocator_t span_allocator;
    color_array_t color_array;
    agg::scanline_u8 scanline;
    double d1 = 0, d2 = 0;

    if (m_type == Paint::k_PaintTypeRadialGradient)
    {
        // length is the radius
        d2 = points[k_RadialR];
    }
    else if (m_type == Paint::k_PaintTypeLinearGradient)
    {
        // length is the distance between the two points
        const double dx = points[k_LinearX2] - points[k_LinearX1];
        const double dy = points[k_LinearY2] - points[k_LinearY1];
        d2 = sqrt(dx * dx + dy * dy);
        
        if (dx == 0.0)
        {
            // gradient_y. handle flips
            gradient_mtx *= agg::trans_affine_rotation(atan2(0.0, dy));
        }
        else if (dy == 0.0)
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

    gradient_mtx *= agg::trans_affine_translation(points[0], points[1]);
    if (m_units == Paint::k_GradientUnitsUserSpace)
    {
        gradient_mtx *= m_transform;
    }
    gradient_mtx.invert();

    _generate_colors<pixfmt_t, color_array_t>(m_stops, color_array);

    span_gradient_t span_gradient(span_interpolator, func, color_array, d1, d2);
    gradient_renderer_t grad_renderer(renderer, span_allocator, span_gradient);
    agg::render_scanlines(ras, scanline, grad_renderer);
}

template <typename pixfmt_t, typename rasterizer_t, typename renderer_t>
void Paint::_render_pattern(rasterizer_t& ras, renderer_t& renderer)
{
    switch (m_pattern_style)
    {
    case k_PatternStyleReflect:
        {
            typedef typename image_filters<pixfmt_t>::source_reflect_t source_t;
            typedef typename image_filters<pixfmt_t>::nearest_reflect_t span_gen_t;

            _render_pattern_final<pixfmt_t, rasterizer_t, renderer_t, source_t, span_gen_t>(ras, renderer);
        }
        break;

    case k_PatternStyleRepeat:
        {
            typedef typename image_filters<pixfmt_t>::source_repeat_t source_t;
            typedef typename image_filters<pixfmt_t>::nearest_repeat_t span_gen_t;

            _render_pattern_final<pixfmt_t, rasterizer_t, renderer_t, source_t, span_gen_t>(ras, renderer);
        }
        break;

    default:
        break;
    }
}

template <typename pixfmt_t, typename rasterizer_t, typename renderer_t, typename source_t, typename span_gen_t>
void Paint::_render_pattern_final(rasterizer_t& ras, renderer_t& renderer)
{
    typedef typename agg::span_allocator<typename pixfmt_t::color_type> span_alloc_t;
    typedef agg::renderer_scanline_aa<renderer_t, span_alloc_t, span_gen_t> img_renderer_t;

    agg::trans_affine inv_img_mtx = m_transform;
    inv_img_mtx.invert();
    interpolator_t interpolator(inv_img_mtx);

    agg::scanline_u8 scanline;
    span_alloc_t span_allocator;
    pixfmt_t src_pix(m_image->get_buffer());
    source_t source(src_pix);
    span_gen_t span_generator(source, interpolator);
    img_renderer_t pattern_renderer(renderer, span_allocator, span_generator);

    agg::render_scanlines(ras, scanline, pattern_renderer);
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
