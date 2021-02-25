from __future__ import unicode_literals

import unittest

import numpy as np

import celiagg as agg


@unittest.skipIf(not agg.HAS_TEXT, 'Text support is not available')
class TestTextDrawing(unittest.TestCase):
    def test_font_byte_string(self):
        canvas = agg.CanvasG8(np.zeros((1, 1), dtype=np.uint8))
        gs = agg.GraphicsState()
        transform = agg.Transform()

        text_unicode = 'Hello!'
        font_unicode = agg.Font(agg.example_font(), 12.0, face_index=1)
        text_byte = b'Hello!'
        font_byte = agg.Font(agg.example_font().encode('utf8'), 12.0)

        canvas.draw_text(text_unicode, font_unicode, transform, gs)
        canvas.draw_text(text_byte, font_unicode, transform, gs)
        canvas.draw_text(text_unicode, font_byte, transform, gs)
        canvas.draw_text(text_byte, font_byte, transform, gs)

    def test_text_rendering(self):
        font_cache = agg.FontCache()
        canvas = agg.CanvasRGB24(
            np.zeros((100, 100, 3), dtype=np.uint8), font_cache=font_cache,
        )
        canvas.clear(1.0, 1.0, 1.0)

        font = agg.Font(agg.example_font(), 24.0)
        gs = agg.GraphicsState()
        paint = agg.SolidPaint(1.0, 0.0, 0.0, 1.0)
        transform = agg.Transform()
        transform.translate(25, 50)

        text = 'Hello!'
        canvas.draw_text(text, font, transform, gs, stroke=paint, fill=paint)

        check = np.sum(canvas.array == [255, 0, 0], axis=2) == 3
        self.assertTrue(check.any())

    def test_text_measurement(self):
        # This is a regression test for issue #62
        # https://github.com/celiagg/celiagg/issues/62

        font_cache = agg.FontCache()
        canvas = agg.CanvasRGB24(
            np.zeros((100, 100, 3), dtype=np.uint8), font_cache=font_cache,
        )
        font = agg.Font(agg.example_font(), 12.0)
        gs = agg.GraphicsState()
        paint = agg.SolidPaint(1.0, 0.0, 0.0, 1.0)
        text = 'Some appropriate string'
        transform = agg.Transform()

        # Measure before drawing
        width_before = font_cache.width(font, text)

        # Draw with a transform
        transform.translate(25, 75)
        transform.rotate(0.75)
        canvas.draw_text(text, font, transform, gs, stroke=paint, fill=paint)

        # Measure after drawing
        width_after = font_cache.width(font, text)

        self.assertEqual(width_after, width_before)
