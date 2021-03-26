// The MIT License (MIT)
//
// Copyright (c) 2014-2015 WUSTL ZPLAB
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
// Authors: Erik Hvatum <ice.rikh@gmail.com>
//          John Wiggins

// This is taken from the "image_alpha.cpp" AGG example
template <typename color_type>
class span_conv_alpha
{
public:
    typedef agg::cover_type alpha_type;

    span_conv_alpha(const alpha_type alpha) : m_alpha(alpha) {}
    void prepare() {}

    void generate(color_type* span, int x, int y, unsigned len) const
    {
        do
        {
            // It's a bit of a hack, but we can treat the 8-bit alpha value as a cover.
            span->a = color_type::mult_cover(color_type::full_value(), m_alpha);
            ++span;
        }
        while(--len);
    }

private:
    const alpha_type m_alpha;
};


template<typename pixfmt_t>
template<typename base_renderer_t>
void canvas<pixfmt_t>::_draw_image_internal(Image& img,
    const agg::trans_affine& transform, const GraphicsState& gs,
    base_renderer_t& renderer)
{
    typedef typename image_filters<pixfmt_t>::source_t source_t;
    typedef agg::conv_transform<agg::path_storage> trans_curve_t;
    typedef span_conv_alpha<typename pixfmt_t::color_type> conv_t;

    pixfmt_t src_pix(img.get_buffer());
    typename pixfmt_t::color_type back_color(agg::rgba(0.5, 0.5, 0.5, 1.0));
    source_t source(src_pix, back_color);

    agg::trans_affine src_mtx = transform;
    agg::trans_affine inv_img_mtx = ~src_mtx;
    interpolator_t interpolator(inv_img_mtx);

    m_rasterizer.reset();
    agg::path_storage img_outline = img.image_outline();
    trans_curve_t trans_outline(img_outline, src_mtx);
    m_rasterizer.add_path(trans_outline);

    switch (gs.image_interpolation_mode())
    {
    case GraphicsState::InterpolationNearest:
        {
            typedef typename image_filters<pixfmt_t>::nearest_t span_gen_t;

            span_gen_t span_generator(source, interpolator);
            if (gs.master_alpha() < 1.0) {
                typedef typename agg::span_converter<span_gen_t, conv_t> span_conv_t;

                conv_t converter(agg::cover_type(gs.master_alpha() * 255));
                span_conv_t span_converer(span_generator, converter);
                _draw_image_final<base_renderer_t>(renderer, span_converer);
            } else {
                _draw_image_final<base_renderer_t>(renderer, span_generator);
            }
            break;
        }
    case GraphicsState::InterpolationBilinear:
        {
            typedef typename image_filters<pixfmt_t>::bilinear_t span_gen_t;

            span_gen_t span_generator(source, interpolator);
            if (gs.master_alpha() < 1.0) {
                typedef typename agg::span_converter<span_gen_t, conv_t> span_conv_t;

                conv_t converter(agg::cover_type(gs.master_alpha() * 255));
                span_conv_t span_converer(span_generator, converter);
                _draw_image_final<base_renderer_t>(renderer, span_converer);
            } else {
                _draw_image_final<base_renderer_t>(renderer, span_generator);
            }
            break;
        }
    case GraphicsState::InterpolationBicubic:
    case GraphicsState::InterpolationSpline16:
    case GraphicsState::InterpolationSpline36:
    case GraphicsState::InterpolationSinc64:
    case GraphicsState::InterpolationSinc144:
    case GraphicsState::InterpolationSinc256:
    case GraphicsState::InterpolationBlackman64:
    case GraphicsState::InterpolationBlackman100:
    case GraphicsState::InterpolationBlackman256:
        {
            typedef typename image_filters<pixfmt_t>::general_t span_gen_t;
            agg::image_filter_lut filter;
            switch (gs.image_interpolation_mode())
            {
                case GraphicsState::InterpolationBicubic:
                    filter.calculate(agg::image_filter_bicubic()); break;
                case GraphicsState::InterpolationSpline16:
                    filter.calculate(agg::image_filter_spline16()); break;
                case GraphicsState::InterpolationSpline36:
                    filter.calculate(agg::image_filter_spline36()); break;
                case GraphicsState::InterpolationSinc64:
                    filter.calculate(agg::image_filter_sinc64()); break;
                case GraphicsState::InterpolationSinc144:
                    filter.calculate(agg::image_filter_sinc144()); break;
                case GraphicsState::InterpolationSinc256:
                    filter.calculate(agg::image_filter_sinc256()); break;
                case GraphicsState::InterpolationBlackman64:
                    filter.calculate(agg::image_filter_blackman64()); break;
                case GraphicsState::InterpolationBlackman100:
                    filter.calculate(agg::image_filter_blackman100()); break;
                case GraphicsState::InterpolationBlackman256:
                    filter.calculate(agg::image_filter_blackman256()); break;
                default: break;
            }

            span_gen_t span_generator(source, interpolator, filter);
            if (gs.master_alpha() < 1.0) {
                typedef typename agg::span_converter<span_gen_t, conv_t> span_conv_t;

                conv_t converter(agg::cover_type(gs.master_alpha() * 255));
                span_conv_t span_converer(span_generator, converter);
                _draw_image_final<base_renderer_t>(renderer, span_converer);
            } else {
                _draw_image_final<base_renderer_t>(renderer, span_generator);
            }
            break;
        }
    }
}

template<typename pixfmt_t>
template<typename base_renderer_t, typename span_gen_t>
void canvas<pixfmt_t>::_draw_image_final(
    base_renderer_t& renderer, span_gen_t& span_generator)
{
    typedef typename agg::span_allocator<typename pixfmt_t::color_type> span_alloc_t;
    typedef agg::renderer_scanline_aa<base_renderer_t, span_alloc_t, span_gen_t> img_renderer_t;

    span_alloc_t span_allocator;
    img_renderer_t img_renderer(renderer, span_allocator, span_generator);
    agg::render_scanlines(m_rasterizer, m_scanline, img_renderer);
}
