from __future__ import division

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
