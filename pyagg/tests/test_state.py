from pyagg import GraphicsState, BlendMode, Color, LineCap, LineJoin, Rect


def test_state_properties():
    gs = GraphicsState()

    gs.anti_aliased = True
    assert gs.anti_aliased is True
    gs.anti_aliased = False
    assert gs.anti_aliased is False
    gs.fill_even_odd = True
    assert gs.fill_even_odd is True
    gs.fill_even_odd = False
    assert gs.fill_even_odd is False

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
