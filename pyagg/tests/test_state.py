from nose.tools import assert_raises
import numpy as np

from pyagg import (GraphicsState, BlendMode, DrawingMode, Image, InnerJoin,
                   LineCap, LineJoin, PixelFormat, Rect, TextDrawingMode)


def array_bases_equal(arr0, arr1):
    return (arr0.__array_interface__['data'][0] ==
            arr1.__array_interface__['data'][0])


def test_rect():
    r = Rect(0, 0, -1, -1)
    assert not r.valid

    r = Rect(2.0, 3.0, 5.0, 6.0)
    assert r.x == 2.0
    assert r.y == 3.0
    assert r.w == 5.0
    assert r.h == 6.0


def test_state_bad_value_types():
    gs = GraphicsState()

    with assert_raises(TypeError):
        gs.clip_box = (0, 1, 2, 3)
    with assert_raises(TypeError):
        gs.clip_box = None

    with assert_raises(TypeError):
        gs.stencil = "dur hur"


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

    gs.text_drawing_mode = TextDrawingMode.TextDrawStroke
    assert gs.text_drawing_mode == TextDrawingMode.TextDrawStroke
    gs.text_drawing_mode = TextDrawingMode.TextDrawFillStroke
    assert gs.text_drawing_mode == TextDrawingMode.TextDrawFillStroke

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

    gs.inner_join = InnerJoin.InnerRound
    assert gs.inner_join == InnerJoin.InnerRound
    gs.inner_join = InnerJoin.InnerBevel
    assert gs.inner_join == InnerJoin.InnerBevel

    gs.miter_limit = 3.14
    assert gs.miter_limit == 3.14
    gs.miter_limit = 0.5
    assert gs.miter_limit == 0.5

    gs.inner_miter_limit = 3.14
    assert gs.inner_miter_limit == 3.14
    gs.inner_miter_limit = 0.5
    assert gs.inner_miter_limit == 0.5

    gs.master_alpha = 0.42
    assert gs.master_alpha == 0.42

    gs.line_width = 10.0
    assert gs.line_width == 10.0

    box = Rect(0.0, 0.0, 10.0, 20.0)
    gs.clip_box = box
    assert gs.clip_box == box
    assert not (gs.clip_box is box)

    dashes = [(1.0, 2.0), (3.0, 4.0)]
    gs.line_dash_pattern = dashes
    gs.line_dash_phase = 5.0
    assert gs.line_dash_pattern == dashes
    assert gs.line_dash_phase == 5.0
    gs.line_dash_pattern = []
    assert len(gs.line_dash_pattern) == 0

    img = Image(np.zeros((10, 10), dtype=np.uint8), PixelFormat.Gray8)
    assert gs.stencil is None
    gs.stencil = img
    assert gs.stencil is img
    gs.stencil = None
    assert gs.stencil is None

    img = Image(np.zeros((10, 10, 3), dtype=np.uint8), PixelFormat.RGB24)
    with assert_raises(ValueError):
        gs.stencil = img


def test_kwargs_initialization():
    box = Rect(0.0, 0.0, 10.0, 20.0)
    dashes = [(1.0, 2.0), (3.0, 4.0)]
    img = Image(np.zeros((10, 10), dtype=np.uint8), PixelFormat.Gray8)
    gs = GraphicsState(
        anti_aliased=True,
        drawing_mode=DrawingMode.DrawEofFill,
        text_drawing_mode=TextDrawingMode.TextDrawFillStroke,
        blend_mode=BlendMode.BlendXor,
        image_blend_mode=BlendMode.BlendXor,
        line_cap=LineCap.CapSquare,
        line_join=LineJoin.JoinRound,
        inner_join=InnerJoin.InnerRound,
        miter_limit=3.14,
        inner_miter_limit=3.14,
        master_alpha=0.42,
        line_width=10.0,
        clip_box=box,
        line_dash_pattern=dashes,
        line_dash_phase=3.5,
        stencil=img
    )

    assert gs.anti_aliased is True
    assert gs.drawing_mode == DrawingMode.DrawEofFill
    assert gs.text_drawing_mode == TextDrawingMode.TextDrawFillStroke
    assert gs.blend_mode == BlendMode.BlendXor
    assert gs.image_blend_mode == BlendMode.BlendXor
    assert gs.line_cap == LineCap.CapSquare
    assert gs.line_join == LineJoin.JoinRound
    assert gs.inner_join == InnerJoin.InnerRound
    assert gs.miter_limit == 3.14
    assert gs.inner_miter_limit == 3.14
    assert gs.master_alpha == 0.42
    assert gs.line_width == 10.0
    assert gs.clip_box == box
    assert gs.line_dash_pattern == dashes
    assert gs.line_dash_phase == 3.5
    assert gs.stencil is img


def test_copy():
    box = Rect(0.0, 0.0, 10.0, 20.0)
    dashes = [(1.0, 2.0), (3.0, 4.0)]
    img = Image(np.zeros((10, 10), dtype=np.uint8), PixelFormat.Gray8)
    gs = GraphicsState(
        anti_aliased=True,
        drawing_mode=DrawingMode.DrawEofFill,
        text_drawing_mode=TextDrawingMode.TextDrawFillStroke,
        blend_mode=BlendMode.BlendXor,
        image_blend_mode=BlendMode.BlendXor,
        line_cap=LineCap.CapSquare,
        line_join=LineJoin.JoinRound,
        inner_join=InnerJoin.InnerRound,
        miter_limit=3.14,
        inner_miter_limit=3.14,
        master_alpha=0.42,
        line_width=10.0,
        clip_box=box,
        line_dash_pattern=dashes,
        line_dash_phase=3.5,
        stencil=img
    )
    cpy = gs.copy()

    assert cpy.anti_aliased is True
    assert cpy.drawing_mode == DrawingMode.DrawEofFill
    assert cpy.text_drawing_mode == TextDrawingMode.TextDrawFillStroke
    assert cpy.blend_mode == BlendMode.BlendXor
    assert cpy.image_blend_mode == BlendMode.BlendXor
    assert cpy.line_cap == LineCap.CapSquare
    assert cpy.line_join == LineJoin.JoinRound
    assert cpy.inner_join == InnerJoin.InnerRound
    assert cpy.miter_limit == 3.14
    assert cpy.inner_miter_limit == 3.14
    assert cpy.master_alpha == 0.42
    assert cpy.line_width == 10.0
    assert cpy.clip_box == box
    assert cpy.line_dash_pattern == dashes
    assert cpy.line_dash_phase == 3.5

    # The image has no comparison operator. Make sure its a new object
    assert cpy.stencil is not img
    assert not array_bases_equal(cpy.stencil.pixels, gs.stencil.pixels)
