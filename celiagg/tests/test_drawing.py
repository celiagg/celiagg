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

import numpy as np
from numpy.testing import assert_equal

import celiagg as agg


class TestDrawing(unittest.TestCase):
    def setUp(self):
        buffer = np.zeros((5, 5), dtype=np.uint8)
        self.canvas = agg.CanvasG8(buffer)
        self.paint = agg.SolidPaint(1/255, 1/255, 1/255)
        self.state = agg.GraphicsState(anti_aliased=False, line_width=1.0)
        self.transform = agg.Transform()

    def test_lines(self):
        path = agg.Path()
        path.move_to(0, .5)
        path.line_to(5, .5)
        expected = [
            [1, 1, 1, 1, 1],
            [0, 0, 0, 0, 0],
            [0, 0, 0, 0, 0],
            [0, 0, 0, 0, 0],
            [0, 0, 0, 0, 0],
        ]

        self.canvas.draw_shape(
            path, self.transform, self.state, stroke=self.paint
        )
        assert_equal(expected, self.canvas.array)

        self.canvas.clear(0, 0, 0)
        path.reset()
        path.move_to(.5, 0)
        path.line_to(.5, 5)
        expected = [
            [1, 0, 0, 0, 0],
            [1, 0, 0, 0, 0],
            [1, 0, 0, 0, 0],
            [1, 0, 0, 0, 0],
            [1, 0, 0, 0, 0],
        ]
        self.canvas.draw_shape(
            path, self.transform, self.state, stroke=self.paint
        )
        assert_equal(expected, self.canvas.array)

        self.canvas.clear(0, 0, 0)
        path.reset()
        path.move_to(0, 0)
        path.line_to(5, 5)
        path.move_to(0, 5)
        path.line_to(5, 0)
        self.state.line_width = 0.5
        expected = [
            [1, 0, 0, 0, 1],
            [0, 1, 0, 1, 0],
            [0, 0, 1, 0, 0],
            [0, 1, 0, 1, 0],
            [1, 0, 0, 0, 1],
        ]
        self.canvas.draw_shape(
            path, self.transform, self.state, stroke=self.paint
        )
        assert_equal(expected, self.canvas.array)

    def test_rect(self):
        path = agg.Path()
        path.rect(0, 0, 5, 5)
        expected = [
            [1, 1, 1, 1, 1],
            [1, 0, 0, 0, 1],
            [1, 0, 0, 0, 1],
            [1, 0, 0, 0, 1],
            [1, 1, 1, 1, 1],
        ]
        self.canvas.draw_shape(
            path, self.transform, self.state, stroke=self.paint
        )
        assert_equal(expected, self.canvas.array)

        expected = [
            [1, 1, 1, 1, 1],
            [1, 1, 1, 1, 1],
            [1, 1, 1, 1, 1],
            [1, 1, 1, 1, 1],
            [1, 1, 1, 1, 1],
        ]
        self.canvas.draw_shape(
            path, self.transform, self.state,
            fill=self.paint, stroke=self.paint
        )
        assert_equal(expected, self.canvas.array)

    def test_ellipse(self):
        path = agg.Path()
        path.ellipse(2.5, 2.5, 2, 2)
        expected = [
            [0, 1, 1, 1, 0],
            [1, 1, 0, 1, 1],
            [1, 0, 0, 0, 1],
            [1, 1, 0, 1, 1],
            [0, 1, 1, 1, 0],
        ]
        self.canvas.draw_shape(
            path, self.transform, self.state, stroke=self.paint
        )
        assert_equal(expected, self.canvas.array)

    def test_draw_shape_at_points(self):
        path = agg.Path()
        path.ellipse(0, 0, 0.5, 0.5)
        points = [(0.0, 0.0), (0.0, 5.0), (5.0, 0.0), (5.0, 5.0), (2.5, 2.5)]
        expected = [
            [1, 0, 0, 0, 1],
            [0, 0, 0, 0, 0],
            [0, 0, 1, 0, 0],
            [0, 0, 0, 0, 0],
            [1, 0, 0, 0, 1],
        ]
        self.canvas.draw_shape_at_points(
            path, points, self.transform, self.state, stroke=self.paint
        )
        assert_equal(expected, self.canvas.array)
