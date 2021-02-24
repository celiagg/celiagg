from __future__ import unicode_literals

import os
import sys
import unittest

import numpy as np

import celiagg as agg


def _get_a_font():
    """ Attempt to find a font file to load
    """
    if sys.platform in ('win32', 'cygwin'):
        directories = [r'C:\Windows\Fonts']
    elif sys.platform == 'darwin':
        directories = ['/System/Library/Fonts']
    else:
        directories = [
            '/usr/X11R6/lib/X11/fonts/TTF/',
            '/usr/share/fonts/',
            '/usr/local/share/fonts/',
        ]

    for directory in directories:
        for name in os.listdir(directory):
            if not name.lower().endswith('.ttf'):
                continue

            return os.path.join(directory, name)

    return 'Mojibake'


class TestTextDrawing(unittest.TestCase):
    def test_font_byte_string(self):
        canvas = agg.CanvasG8(np.zeros((1, 1), dtype=np.uint8))
        gs = agg.GraphicsState()
        transform = agg.Transform()

        text_unicode = 'Hello!'
        font_unicode = agg.Font(
            'Times New Roman',
            12.0,
            agg.FontCacheType.RasterFontCache,
            face_index=1,
        )
        text_byte = b'Hello!'
        font_byte = agg.Font(
            b'Times New Roman',
            12.0,
            agg.FontCacheType.RasterFontCache,
        )

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

        # This loads a random font
        font_name = _get_a_font()
        font = agg.Font(font_name, 24.0, agg.FontCacheType.RasterFontCache)
        gs = agg.GraphicsState()
        paint = agg.SolidPaint(1.0, 0.0, 0.0, 1.0)
        transform = agg.Transform()
        transform.translate(25, 50)

        text = 'Hello!'
        canvas.draw_text(text, font, transform, gs, stroke=paint, fill=paint)

        check = np.sum(canvas.array == [255, 0, 0], axis=2) == 3
        self.assertTrue(check.any(),
                        f'Failed to render with font: {font_name}')

    def test_text_measurement(self):
        # This is a regression test for issue #62
        # https://github.com/celiagg/celiagg/issues/62

        font_cache = agg.FontCache()
        canvas = agg.CanvasRGB24(
            np.zeros((100, 100, 3), dtype=np.uint8), font_cache=font_cache,
        )
        font_name = _get_a_font()
        font = agg.Font(font_name, 12.0, agg.FontCacheType.RasterFontCache)
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
