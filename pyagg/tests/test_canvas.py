import numpy as np
from numpy.testing import assert_equal

from pyagg import GraphicsState, Color, ndarray_canvas_rgb24


def test_line():
    expected = np.zeros((100, 100, 3), dtype=np.uint8)
    buffer = np.zeros((100, 100, 3), dtype=np.uint8)
    canvas = ndarray_canvas_rgb24(buffer)
    gs = GraphicsState()

    gs.anti_aliased = False
    gs.line_color = Color(10, 10, 10)
    gs.line_width = 1.0

    expected[1, ...] = 10
    canvas.draw_line(0, 1.5, 100, 1.5, gs)
    assert_equal(expected, canvas.image)

    buffer[:] = 0
    expected[:] = 0

    expected[:, 1, :] = 10
    canvas.draw_line(1.5, 0, 1.5, 100, gs)
    assert_equal(expected, buffer)
