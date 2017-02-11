from __future__ import division

import numpy as np
from numpy.testing import assert_equal
import pytest

import celiagg as agg


@pytest.fixture
def canvas():
    buffer = np.zeros((5, 5), dtype=np.uint8)
    return agg.CanvasG8(buffer)


@pytest.fixture
def paint():
    return agg.SolidPaint(1/255, 1/255, 1/255)


@pytest.fixture
def state():
    return agg.GraphicsState(anti_aliased=False, line_width=1.0)


@pytest.fixture
def transform():
    return agg.Transform()


def test_lines(canvas, paint, state, transform):
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

    canvas.draw_shape(path, transform, state, stroke=paint)
    assert_equal(expected, canvas.array)

    canvas.clear(0, 0, 0)
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
    canvas.draw_shape(path, transform, state, stroke=paint)
    assert_equal(expected, canvas.array)

    canvas.clear(0, 0, 0)
    path.reset()
    path.move_to(0, 0)
    path.line_to(5, 5)
    path.move_to(0, 5)
    path.line_to(5, 0)
    state.line_width = 0.5
    expected = [
        [1, 0, 0, 0, 1],
        [0, 1, 0, 1, 0],
        [0, 0, 1, 0, 0],
        [0, 1, 0, 1, 0],
        [1, 0, 0, 0, 1],
    ]
    canvas.draw_shape(path, transform, state, stroke=paint)
    assert_equal(expected, canvas.array)


def test_rect(canvas, paint, state, transform):
    path = agg.Path()
    path.rect(0, 0, 5, 5)
    expected = [
        [1, 1, 1, 1, 1],
        [1, 0, 0, 0, 1],
        [1, 0, 0, 0, 1],
        [1, 0, 0, 0, 1],
        [1, 1, 1, 1, 1],
    ]
    canvas.draw_shape(path, transform, state, stroke=paint)
    assert_equal(expected, canvas.array)

    expected = [
        [1, 1, 1, 1, 1],
        [1, 1, 1, 1, 1],
        [1, 1, 1, 1, 1],
        [1, 1, 1, 1, 1],
        [1, 1, 1, 1, 1],
    ]
    canvas.draw_shape(path, transform, state, fill=paint, stroke=paint)
    assert_equal(expected, canvas.array)


def test_ellipse(canvas, paint, state, transform):
    path = agg.Path()
    path.ellipse(2.5, 2.5, 2, 2)
    expected = [
        [0, 1, 1, 1, 0],
        [1, 1, 0, 1, 1],
        [1, 0, 0, 0, 1],
        [1, 1, 0, 1, 1],
        [0, 1, 1, 1, 0],
    ]
    canvas.draw_shape(path, transform, state, stroke=paint)
    assert_equal(expected, canvas.array)
