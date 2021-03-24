# The MIT License (MIT)
#
# Copyright (c) 2016-2021 Celiagg Contributors
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

        if agg.HAS_TEXT:
            text = 'Hello!'
            font = agg.Font(agg.example_font(), 12.0)
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

    def test_rasterizer_cell_overflow(self):
        canvas = agg.CanvasRGB24(np.zeros((100, 100, 3), dtype=np.uint8))
        gs = agg.GraphicsState()
        transform = agg.Transform()
        path = agg.Path()

        def genpoints(num):
            arr = np.empty((num, 2))
            arr[::2, 0] = np.linspace(0, 99, num=arr[::2, 0].shape[0])
            arr[::2, 1] = 0.0
            arr[1::2, 0] = np.linspace(1, 100, num=arr[1::2, 0].shape[0])
            arr[1::2, 1] = 100.0
            return arr

        # This many points definitely generates more than 2^22 cells
        count = 2**22 // 100 // 2
        points = genpoints(count)
        path.lines(points)
        with self.assertRaises(OverflowError):
            canvas.draw_shape(path, transform, gs)

        path.reset()

        # This many points is OK
        count = 2**22 // 103 // 2
        points = genpoints(count)
        path.lines(points)
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
