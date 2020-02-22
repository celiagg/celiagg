import unittest

import numpy as np

from celiagg import (GraphicsState, BlendMode, DrawingMode, Image, InnerJoin,
                     LineCap, LineJoin, PixelFormat, Rect, TextDrawingMode)


def array_bases_equal(arr0, arr1):
    return (arr0.__array_interface__['data'][0] ==
            arr1.__array_interface__['data'][0])


class TestGraphicsState(unittest.TestCase):
    def test_rect(self):
        r = Rect(0, 0, -1, -1)
        self.assertFalse(r.valid)

        r = Rect(2.0, 3.0, 5.0, 6.0)
        self.assertEqual(r.x, 2.0)
        self.assertEqual(r.y, 3.0)
        self.assertEqual(r.w, 5.0)
        self.assertEqual(r.h, 6.0)

        gs = GraphicsState()
        self.assertFalse(gs.clip_box.valid)

    def test_state_bad_value_types(self):
        gs = GraphicsState()

        with self.assertRaises(TypeError):
            gs.clip_box = (0, 1, 2, 3)
        with self.assertRaises(TypeError):
            gs.clip_box = None

        with self.assertRaises(TypeError):
            gs.stencil = 'dur hur'

    def test_state_properties(self):
        gs = GraphicsState()

        gs.anti_aliased = True
        self.assertTrue(gs.anti_aliased)
        gs.anti_aliased = False
        self.assertFalse(gs.anti_aliased)

        gs.drawing_mode = DrawingMode.DrawEofFill
        self.assertEqual(gs.drawing_mode, DrawingMode.DrawEofFill)
        gs.drawing_mode = DrawingMode.DrawFillStroke
        self.assertEqual(gs.drawing_mode, DrawingMode.DrawFillStroke)

        gs.text_drawing_mode = TextDrawingMode.TextDrawStroke
        self.assertEqual(gs.text_drawing_mode,
                         TextDrawingMode.TextDrawStroke)
        gs.text_drawing_mode = TextDrawingMode.TextDrawFillStroke
        self.assertEqual(gs.text_drawing_mode,
                         TextDrawingMode.TextDrawFillStroke)

        gs.blend_mode = BlendMode.BlendXor
        self.assertEqual(gs.blend_mode, BlendMode.BlendXor)
        gs.blend_mode = BlendMode.BlendLighten
        self.assertEqual(gs.blend_mode, BlendMode.BlendLighten)

        gs.image_blend_mode = BlendMode.BlendXor
        self.assertEqual(gs.image_blend_mode, BlendMode.BlendXor)
        gs.image_blend_mode = BlendMode.BlendLighten
        self.assertEqual(gs.image_blend_mode, BlendMode.BlendLighten)

        gs.line_cap = LineCap.CapSquare
        self.assertEqual(gs.line_cap, LineCap.CapSquare)
        gs.line_cap = LineCap.CapRound
        self.assertEqual(gs.line_cap, LineCap.CapRound)

        gs.line_join = LineJoin.JoinRound
        self.assertEqual(gs.line_join, LineJoin.JoinRound)
        gs.line_join = LineJoin.JoinBevel
        self.assertEqual(gs.line_join, LineJoin.JoinBevel)

        gs.inner_join = InnerJoin.InnerRound
        self.assertEqual(gs.inner_join, InnerJoin.InnerRound)
        gs.inner_join = InnerJoin.InnerBevel
        self.assertEqual(gs.inner_join, InnerJoin.InnerBevel)

        gs.miter_limit = 3.14
        self.assertEqual(gs.miter_limit, 3.14)
        gs.miter_limit = 0.5
        self.assertEqual(gs.miter_limit, 0.5)

        gs.inner_miter_limit = 3.14
        self.assertEqual(gs.inner_miter_limit, 3.14)
        gs.inner_miter_limit = 0.5
        self.assertEqual(gs.inner_miter_limit, 0.5)

        gs.master_alpha = 0.42
        self.assertEqual(gs.master_alpha, 0.42)

        gs.line_width = 10.0
        self.assertEqual(gs.line_width, 10.0)

        box = Rect(0.0, 0.0, 10.0, 20.0)
        gs.clip_box = box
        self.assertEqual(gs.clip_box, box)
        self.assertIsNot(gs.clip_box, box)

        dashes = [(1.0, 2.0), (3.0, 4.0)]
        gs.line_dash_pattern = dashes
        gs.line_dash_phase = 5.0
        self.assertEqual(gs.line_dash_pattern, dashes)
        self.assertEqual(gs.line_dash_phase, 5.0)
        gs.line_dash_pattern = []
        self.assertEqual(len(gs.line_dash_pattern), 0)

        img = Image(np.zeros((10, 10), dtype=np.uint8), PixelFormat.Gray8)
        self.assertIsNone(gs.stencil)
        gs.stencil = img
        self.assertIs(gs.stencil, img)
        gs.stencil = None
        self.assertIsNone(gs.stencil)

        img = Image(np.zeros((10, 10, 3), dtype=np.uint8), PixelFormat.RGB24)
        with self.assertRaises(ValueError):
            gs.stencil = img

    def test_kwargs_initialization(self):
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

        self.assertTrue(gs.anti_aliased)
        self.assertEqual(gs.drawing_mode, DrawingMode.DrawEofFill)
        self.assertEqual(gs.text_drawing_mode,
                         TextDrawingMode.TextDrawFillStroke)
        self.assertEqual(gs.blend_mode, BlendMode.BlendXor)
        self.assertEqual(gs.image_blend_mode, BlendMode.BlendXor)
        self.assertEqual(gs.line_cap, LineCap.CapSquare)
        self.assertEqual(gs.line_join, LineJoin.JoinRound)
        self.assertEqual(gs.inner_join, InnerJoin.InnerRound)
        self.assertEqual(gs.miter_limit, 3.14)
        self.assertEqual(gs.inner_miter_limit, 3.14)
        self.assertEqual(gs.master_alpha, 0.42)
        self.assertEqual(gs.line_width, 10.0)
        self.assertEqual(gs.clip_box, box)
        self.assertEqual(gs.line_dash_pattern, dashes)
        self.assertEqual(gs.line_dash_phase, 3.5)
        self.assertIs(gs.stencil, img)

    def test_copy(self):
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

        self.assertTrue(cpy.anti_aliased)
        self.assertEqual(cpy.drawing_mode, DrawingMode.DrawEofFill)
        self.assertEqual(cpy.text_drawing_mode,
                         TextDrawingMode.TextDrawFillStroke)
        self.assertEqual(cpy.blend_mode, BlendMode.BlendXor)
        self.assertEqual(cpy.image_blend_mode, BlendMode.BlendXor)
        self.assertEqual(cpy.line_cap, LineCap.CapSquare)
        self.assertEqual(cpy.line_join, LineJoin.JoinRound)
        self.assertEqual(cpy.inner_join, InnerJoin.InnerRound)
        self.assertEqual(cpy.miter_limit, 3.14)
        self.assertEqual(cpy.inner_miter_limit, 3.14)
        self.assertEqual(cpy.master_alpha, 0.42)
        self.assertEqual(cpy.line_width, 10.0)
        self.assertEqual(cpy.clip_box, box)
        self.assertEqual(cpy.line_dash_pattern, dashes)
        self.assertEqual(cpy.line_dash_phase, 3.5)

        # The image has no comparison operator. Make sure it's a new object
        self.assertIsNot(cpy.stencil, img)
        self.assertFalse(
            array_bases_equal(cpy.stencil.pixels, gs.stencil.pixels)
        )
