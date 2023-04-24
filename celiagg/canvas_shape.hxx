// The MIT License (MIT)
//
// Copyright (c) 2014-2015 WUSTL ZPLAB
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
// Authors: Erik Hvatum <ice.rikh@gmail.com>
//          John Wiggins

template<typename pixfmt_t>
template<typename base_renderer_t>
void canvas<pixfmt_t>::_draw_shape_internal(VertexSource& shape,
    const agg::trans_affine& transform, Paint& linePaint, Paint& fillPaint,
    const GraphicsState& gs, base_renderer_t& renderer)
{
    typedef agg::conv_transform<VertexSource> conv_trans_t;
    typedef agg::conv_contour<conv_trans_t> contour_shape_t;
    typedef agg::scanline_u8 scanline_t;

    const GraphicsState::DrawingMode mode = gs.drawing_mode();
    const bool line = (mode & GraphicsState::DrawStroke) == GraphicsState::DrawStroke;
    const bool fill = (mode & GraphicsState::DrawFill) == GraphicsState::DrawFill;
    const bool eof = (mode & GraphicsState::DrawEofFill) == GraphicsState::DrawEofFill;

    if (line || fill)
    {
        _set_aa(gs.anti_aliased());
        scanline_t scanline;

        if (fill)
        {
            agg::trans_affine mtx = transform;
            conv_trans_t trans_shape(shape, mtx);
            contour_shape_t contour(trans_shape);
            contour.auto_detect_orientation(true);

            m_rasterizer.reset();
            m_rasterizer.add_path(contour);
            m_rasterizer.filling_rule(eof ? agg::fill_even_odd : agg::fill_non_zero);
            fillPaint.render<pixfmt_t, rasterizer_t, scanline_t, base_renderer_t>(m_rasterizer, scanline, renderer, mtx);
        }

        if (line)
        {
            // Handle dashing and other such details
            _draw_shape_stroke_setup(shape, transform, linePaint, gs, renderer);
        }
    }
}

template<typename pixfmt_t>
template<typename base_renderer_t>
void canvas<pixfmt_t>::_draw_shape_stroke_setup(VertexSource& shape,
    const agg::trans_affine& mtx, Paint& paint, const GraphicsState& gs,
    base_renderer_t& renderer)
{
    typedef agg::conv_dash<VertexSource> dash_t;
    typedef agg::conv_stroke<dash_t> dash_stroke_t;
    typedef agg::conv_stroke<VertexSource> stroke_t;

    if (gs.line_dash_pattern().size() > 0)
    {
        typedef GraphicsState::DashPattern::size_type counter_t;

        dash_t dash(shape);
        dash_stroke_t stroke(dash);
        const GraphicsState::DashPattern& dashPattern = gs.line_dash_pattern();

        for (counter_t i=0; i < dashPattern.size(); i+=2)
            dash.add_dash(dashPattern[i], dashPattern[i+1]);
        dash.dash_start(0.0);

        _draw_shape_stroke_final(stroke, mtx, paint, gs, renderer);
    }
    else
    {
        stroke_t stroke(shape);
        _draw_shape_stroke_final(stroke, mtx, paint, gs, renderer);
    }
}

template<typename pixfmt_t>
template<typename stroke_t, typename base_renderer_t>
void canvas<pixfmt_t>::_draw_shape_stroke_final(stroke_t& stroke,
    const agg::trans_affine& mtx, Paint& paint, const GraphicsState& gs,
    base_renderer_t& renderer)
{
    typedef agg::conv_transform<stroke_t> trans_stroke_t;
    typedef agg::scanline_u8 scanline_t;
    scanline_t scanline;

    stroke.width(gs.line_width());
    stroke.miter_limit(gs.miter_limit());
    stroke.inner_miter_limit(gs.inner_miter_limit());
    stroke.line_cap(agg::line_cap_e(gs.line_cap()));
    stroke.line_join(agg::line_join_e(gs.line_join()));
    stroke.inner_join(agg::inner_join_e(gs.inner_join()));

    agg::trans_affine src_mtx = mtx;
    trans_stroke_t trans(stroke, src_mtx);

    m_rasterizer.reset();
    m_rasterizer.add_path(trans);
    paint.render<pixfmt_t, rasterizer_t, scanline_t, base_renderer_t>(m_rasterizer, scanline, renderer, mtx);
}
