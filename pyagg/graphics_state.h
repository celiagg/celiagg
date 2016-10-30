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

#ifndef PYAGG_GRAPHICS_STATE_H
#define PYAGG_GRAPHICS_STATE_H

#include <vector>
#include <agg_basics.h>
#include <agg_math_stroke.h>
#include <agg_pixfmt_rgba.h>

#include "image.h"

class GraphicsState
{
public:
    typedef agg::rect_d Rect;
    typedef std::vector<double> DashPattern;

    enum InnerJoin
    {
        InnerBevel = agg::inner_bevel,
        InnerMiter = agg::inner_miter,
        InnerJag = agg::inner_jag,
        InnerRound = agg::inner_round
    };

    enum LineJoin
    {
        JoinMiter = agg::miter_join,
        JoinRound = agg::round_join,
        JoinBevel = agg::bevel_join
    };

    enum LineCap
    {
        CapButt   = agg::butt_cap,
        CapSquare = agg::square_cap,
        CapRound  = agg::round_cap
    };

    enum DrawingMode
    {
        // Bit 0: fill, Bit 1: stroke, Bit 2: EO flag
        DrawInvisible = 0x0000,
        DrawFill = 0x0001,
        DrawStroke = 0x0002,
        DrawEofFill = 0x0004 | DrawFill,
        DrawFillStroke = DrawFill | DrawStroke,
        DrawEofFillStroke = DrawEofFill | DrawStroke
    };

    enum TextDrawingMode
    {
        // Bit 0: fill, Bit 1: stroke, Bit 2: clip
        TextDrawInvisible = 0x0000,
        TextDrawFill = 0x0001,
        TextDrawStroke = 0x0002,
        TextDrawClip = 0x0004,
        TextDrawFillStroke = TextDrawFill | TextDrawStroke,
        TextDrawFillClip = TextDrawFill | TextDrawClip,
        TextDrawStrokeClip = TextDrawStroke | TextDrawClip,
        TextDrawFillStrokeClip = TextDrawFill | TextDrawStroke | TextDrawClip,
    };

    enum BlendMode
    {
        BlendAlpha      = agg::end_of_comp_op_e,
        BlendClear      = agg::comp_op_clear,
        BlendSrc        = agg::comp_op_src,
        BlendDst        = agg::comp_op_dst,
        BlendSrcOver    = agg::comp_op_src_over,
        BlendDstOver    = agg::comp_op_dst_over,
        BlendSrcIn      = agg::comp_op_src_in,
        BlendDstIn      = agg::comp_op_dst_in,
        BlendSrcOut     = agg::comp_op_src_out,
        BlendDstOut     = agg::comp_op_dst_out,
        BlendSrcAtop    = agg::comp_op_src_atop,
        BlendDstAtop    = agg::comp_op_dst_atop,
        BlendXor        = agg::comp_op_xor,
        BlendAdd        = agg::comp_op_plus,
        BlendMultiply   = agg::comp_op_multiply,
        BlendScreen     = agg::comp_op_screen,
        BlendOverlay    = agg::comp_op_overlay,
        BlendDarken     = agg::comp_op_darken,
        BlendLighten    = agg::comp_op_lighten,
        BlendColorDodge = agg::comp_op_color_dodge,
        BlendColorBurn  = agg::comp_op_color_burn,
        BlendHardLight  = agg::comp_op_hard_light,
        BlendSoftLight  = agg::comp_op_soft_light,
        BlendDifference = agg::comp_op_difference,
        BlendExclusion  = agg::comp_op_exclusion,
    };

    GraphicsState() :
        m_clip_box(0, 0, 0, 0),
        m_stencil(NULL),
        m_drawing_mode(DrawFillStroke),
        m_text_drawing_mode(TextDrawFill),
        m_blend_mode(BlendAlpha),
        m_image_blend_mode(BlendDst),
        m_master_alpha(1.0),
        m_line_dash_phase(0.0),
        m_miter_limit(1.0),
        m_inner_miter_limit(1.0),
        m_line_width(1.0),
        m_line_cap(CapSquare),
        m_line_join(JoinMiter),
        m_inner_join(InnerMiter),
        m_anti_aliased(true)
        {}

    void anti_aliased(bool aa) { m_anti_aliased = aa; }
    bool anti_aliased() const { return m_anti_aliased; }

    void clip_box(Rect r) { m_clip_box = r; }
    void clip_box(double x1, double y1, double x2, double y2) { clip_box(Rect(x1, y1, x2, y2)); }
    Rect clip_box() const { return m_clip_box; }

    void drawing_mode(DrawingMode m) { m_drawing_mode = m; }
    DrawingMode drawing_mode() const { return m_drawing_mode; }

    void text_drawing_mode(TextDrawingMode tm) { m_text_drawing_mode = tm; }
    TextDrawingMode text_drawing_mode() const { return m_text_drawing_mode; }

    void blend_mode(BlendMode m) { m_blend_mode = m; }
    BlendMode blend_mode() const { return m_blend_mode; }

    void image_blend_mode(BlendMode m) { m_image_blend_mode = m; }
    BlendMode image_blend_mode() const { return m_image_blend_mode; }

    void master_alpha(double a) { m_master_alpha = a; }
    double master_alpha() const { return m_master_alpha; }

    void line_width(double w) { m_line_width = w; }
    double line_width() const { return m_line_width; }

    void line_cap(LineCap cap) { m_line_cap = cap; }
    LineCap line_cap() const { return m_line_cap; }

    void line_join(LineJoin join) { m_line_join = join; }
    LineJoin line_join() const { return m_line_join; }

    void inner_join(InnerJoin join) { m_inner_join = join; }
    InnerJoin inner_join() const { return m_inner_join; }

    void miter_limit(double limit) { m_miter_limit = limit; }
    double miter_limit() const { return m_miter_limit; }

    void inner_miter_limit(double limit) { m_inner_miter_limit = limit; }
    double inner_miter_limit() const { return m_inner_miter_limit; }

    void line_dash_pattern(const double *dashes, size_t count)
    {
        m_dashes.clear();
        m_dashes.reserve(count * 2);
        for (size_t i=0; i < count; ++i)
        {
            m_dashes.push_back(dashes[i*2]);
            m_dashes.push_back(dashes[i*2 + 1]);
        }
    }
    const DashPattern& line_dash_pattern() const { return m_dashes; }

    void line_dash_phase(const double phase) { m_line_dash_phase = phase; }
    double line_dash_phase() const { return m_line_dash_phase; }

    void stencil(const Image* image) { m_stencil = image; }
    const Image* stencil() const { return m_stencil; }

private:
    Rect            m_clip_box;
    DashPattern     m_dashes;
    const Image*    m_stencil;
    DrawingMode     m_drawing_mode;
    TextDrawingMode m_text_drawing_mode;
    BlendMode       m_blend_mode;
    BlendMode       m_image_blend_mode;
    double          m_master_alpha;
    double          m_line_dash_phase;
    double          m_miter_limit;
    double          m_inner_miter_limit;
    double          m_line_width;
    LineCap         m_line_cap;
    LineJoin        m_line_join;
    InnerJoin       m_inner_join;
    bool            m_anti_aliased;
};

#endif // PYAGG_GRAPHICS_STATE_H
