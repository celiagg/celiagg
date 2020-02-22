import unittest
import weakref

import numpy as np
from numpy.testing import assert_equal

import celiagg as agg


class TestCanvas(unittest.TestCase):
    def test_cleanup(self):
        # Make sure our array references don't linger
        buffer = np.zeros((10, 10, 3), dtype=np.uint8)
        canvas = agg.CanvasRGB24(buffer)

        weak_buf = weakref.ref(buffer)
        weak_arr = weakref.ref(canvas.array)
        self.assertIs(weak_buf(), weak_arr())

        del buffer
        self.assertIsNotNone(weak_buf())

        del canvas
        self.assertIsNone(weak_buf())

    def test_bad_method_args(self):
        canvas = agg.CanvasG8(np.zeros((1, 1), dtype=np.uint8))
        pix_format = agg.PixelFormat.Gray8
        gs = agg.GraphicsState()
        path = agg.Path()
        transform = agg.Transform()

        with self.assertRaises(TypeError):
            canvas.draw_image(None, pix_format, transform, gs)
        with self.assertRaises(TypeError):
            canvas.draw_image(canvas.array, None, transform, gs)
        with self.assertRaises(TypeError):
            canvas.draw_image(canvas.array, pix_format, None, gs)
        with self.assertRaises(TypeError):
            canvas.draw_image(canvas.array, pix_format, transform, None)
        # But this version should work
        canvas.draw_image(canvas.image, None, transform, gs)

        with self.assertRaises(TypeError):
            canvas.draw_shape(None, transform, gs)
        with self.assertRaises(TypeError):
            canvas.draw_shape(path, None, gs)
        with self.assertRaises(TypeError):
            canvas.draw_shape(path, transform, None)

        text = 'Hello!'
        font = agg.Font(
            'Times New Roman', 12.0, agg.FontCacheType.RasterFontCache
        )
        with self.assertRaises(TypeError):
            canvas.draw_text(text, None, transform, gs)
        with self.assertRaises(TypeError):
            canvas.draw_text(text, font, None, gs)
        with self.assertRaises(TypeError):
            canvas.draw_text(text, font, transform, None)
        canvas.draw_text(text, font, transform, gs)

    def test_stencil_size_mismatch(self):
        canvas = agg.CanvasRGB24(np.zeros((4, 5, 3), dtype=np.uint8))
        stencil_canvas = agg.CanvasG8(np.zeros((1, 2), dtype=np.uint8))
        gs = agg.GraphicsState(stencil=stencil_canvas.image)
        path = agg.Path()
        transform = agg.Transform()

        with self.assertRaises(agg.AggError):
            canvas.draw_shape(path, transform, gs)

    def test_clear(self):
        expected = np.zeros((4, 4, 3), dtype=np.uint8)
        buffer = np.zeros((4, 4, 3), dtype=np.uint8)
        canvas = agg.CanvasRGB24(buffer)

        expected[:] = 0
        canvas.clear(0, 0, 0)
        assert_equal(expected, canvas.array)

        expected[:] = 128
        canvas.clear(.5, .5, .5)
        assert_equal(expected, canvas.array)

        expected[:] = 255
        canvas.clear(1, 1, 1)
        assert_equal(expected, canvas.array)
