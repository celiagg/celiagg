import unittest

import numpy as np

import celiagg as agg


@unittest.skipIf(agg.HAS_TEXT, 'Text support is available')
class TestNoTextSupport(unittest.TestCase):
    def test_no_text_font_failure(self):
        with self.assertRaises(RuntimeError):
            agg.Font()

    def test_no_text_draw_text_failure(self):
        buffer = np.zeros((1, 1), dtype=np.uint8)
        canvas = agg.CanvasG8(buffer)
        transform = agg.Transform()
        line_paint = agg.SolidPaint(1.0, 1.0, 1.0)
        gs = agg.GraphicsState()

        with self.assertRaises(RuntimeError):
            canvas.draw_text('', None, transform, line_paint, line_paint, gs)
