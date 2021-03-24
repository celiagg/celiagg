# The MIT License (MIT)
#
# Copyright (c) 2016-2023 Celiagg Contributors
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
# Authors: John Wiggins
import unittest

import numpy as np

import celiagg as agg


@unittest.skipIf(not agg.HAS_TEXT, 'Text support is not available')
class TestTextDrawing(unittest.TestCase):
    def test_font_byte_string(self):
        canvas = agg.CanvasG8(np.zeros((1, 1), dtype=np.uint8))
        gs = agg.GraphicsState()
        transform = agg.Transform()

        with agg.example_font() as font_path:
            text_unicode = 'Hello!'
            font_unicode = agg.Font(font_path, 12.0)
            text_byte = b'Hello!'
            font_byte = agg.Font(font_path.encode('utf8'), 12.0)

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

        with agg.example_font() as font_path:
            font = agg.Font(font_path, 24.0)
            gs = agg.GraphicsState()
            paint = agg.SolidPaint(1.0, 0.0, 0.0, 1.0)
            transform = agg.Transform()

            text = 'Hello!'
            width = font_cache.width(font, text)
            draw_x, draw_y = 25.0, 50.0
            transform.translate(draw_x, draw_y)
            cursor = canvas.draw_text(
                text, font, transform, gs, stroke=paint, fill=paint
            )
        self.assertIsNotNone(cursor)
        # Text cursor Y is unchanged, X is `width` pixels away from `draw_x`
        self.assertAlmostEqual(cursor[0] - width, draw_x)
        self.assertEqual(cursor[1], draw_y)

        check = np.sum(canvas.array == [255, 0, 0], axis=2) == 3
        self.assertTrue(check.any())

    def test_text_measurement(self):
        # This is a regression test for issue #62
        # https://github.com/celiagg/celiagg/issues/62

        font_cache = agg.FontCache()
        canvas = agg.CanvasRGB24(
            np.zeros((100, 100, 3), dtype=np.uint8), font_cache=font_cache,
        )
        gs = agg.GraphicsState()
        paint = agg.SolidPaint(1.0, 0.0, 0.0, 1.0)
        text = 'Some appropriate string'
        transform = agg.Transform()

        with agg.example_font() as font_path:
            font = agg.Font(font_path, 12.0)

            # Measure before drawing
            width_before = font_cache.width(font, text)

            # Draw with a transform
            transform.translate(25, 75)
            transform.rotate(0.75)
            canvas.draw_text(
                text, font, transform, gs, stroke=paint, fill=paint
            )

            # Measure after drawing
            width_after = font_cache.width(font, text)

        self.assertEqual(width_after, width_before)
