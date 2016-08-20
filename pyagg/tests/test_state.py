from pyagg import (GraphicsState, BlendMode, Color, DrawingMode, LineCap,
                   LineJoin, Rect)


def test_state_properties():
    gs = GraphicsState()

    gs.anti_aliased = True
    assert gs.anti_aliased is True
    gs.anti_aliased = False
    assert gs.anti_aliased is False

    gs.drawing_mode = DrawingMode.DrawEofFill
    assert gs.drawing_mode == DrawingMode.DrawEofFill
    gs.drawing_mode = DrawingMode.DrawFillStroke
    assert gs.drawing_mode == DrawingMode.DrawFillStroke

    gs.blend_mode = BlendMode.BlendXor
    assert gs.blend_mode == BlendMode.BlendXor
    gs.blend_mode = BlendMode.BlendLighten
    assert gs.blend_mode == BlendMode.BlendLighten

    gs.image_blend_mode = BlendMode.BlendXor
    assert gs.image_blend_mode == BlendMode.BlendXor
    gs.image_blend_mode = BlendMode.BlendLighten
    assert gs.image_blend_mode == BlendMode.BlendLighten

    gs.line_cap = LineCap.CapSquare
    assert gs.line_cap == LineCap.CapSquare
    gs.line_cap = LineCap.CapRound
    assert gs.line_cap == LineCap.CapRound

    gs.line_join = LineJoin.JoinRound
    assert gs.line_join == LineJoin.JoinRound
    gs.line_join = LineJoin.JoinBevel
    assert gs.line_join == LineJoin.JoinBevel

    gs.master_alpha = 0.42
    assert gs.master_alpha == 0.42
    gs.anti_alias_gamma = 1.337
    assert gs.anti_alias_gamma == 1.337
    gs.line_width = 10.0
    assert gs.line_width == 10.0

    gs.fill_color = Color(255, 0, 0)
    assert gs.fill_color == Color(255, 0, 0)
    gs.line_color = Color(255, 255, 0)
    assert gs.line_color == Color(255, 255, 0)
    gs.image_blend_color = Color(255, 0, 255)
    assert gs.image_blend_color == Color(255, 0, 255)

    box = Rect(0.0, 0.0, 10.0, 20.0)
    gs.clip_box = box
    assert gs.clip_box == box
    assert not (gs.clip_box is box)


def test_kwargs_initialization():
    box = Rect(0.0, 0.0, 10.0, 20.0)
    red = Color(255, 0, 0)
    yellow = Color(255, 255, 0)
    magenta = Color(255, 0, 255)
    gs = GraphicsState(
        anti_aliased=True,
        drawing_mode=DrawingMode.DrawEofFill,
        blend_mode=BlendMode.BlendXor,
        image_blend_mode=BlendMode.BlendXor,
        line_cap=LineCap.CapSquare,
        line_join=LineJoin.JoinRound,
        master_alpha=0.42,
        anti_alias_gamma=1.337,
        line_width=10.0,
        fill_color=red,
        line_color=yellow,
        image_blend_color=magenta,
        clip_box=box
    )

    assert gs.anti_aliased is True
    assert gs.drawing_mode == DrawingMode.DrawEofFill
    assert gs.blend_mode == BlendMode.BlendXor
    assert gs.image_blend_mode == BlendMode.BlendXor
    assert gs.line_cap == LineCap.CapSquare
    assert gs.line_join == LineJoin.JoinRound
    assert gs.master_alpha == 0.42
    assert gs.anti_alias_gamma == 1.337
    assert gs.line_width == 10.0
    assert gs.fill_color == red
    assert gs.line_color == yellow
    assert gs.image_blend_color == magenta
    assert gs.clip_box == box
