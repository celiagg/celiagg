import weakref

from nose.tools import assert_raises
import numpy as np
from numpy.testing import assert_equal

from pyagg import (AggError, CanvasG8, CanvasRGB24, GraphicsState, Path,
                   SolidPaint, Transform)


def test_cleanup():
    # Make sure our array references don't linger
    buffer = np.zeros((10, 10, 3), dtype=np.uint8)
    canvas = CanvasRGB24(buffer)

    weak_buf = weakref.ref(buffer)
    weak_arr = weakref.ref(canvas.array)

    assert weak_buf() is weak_arr()

    del buffer
    assert weak_buf() is not None

    del canvas
    assert weak_buf() is None


def test_stencil_size_mismatch():
    canvas = CanvasRGB24(np.zeros((4, 5, 3), dtype=np.uint8))
    stencil_canvas = CanvasG8(np.zeros((1, 2), dtype=np.uint8))
    gs = GraphicsState(stencil=stencil_canvas.image)
    path = Path()
    transform = Transform()
    paint = SolidPaint(1.0, 1.0, 1.0)

    with assert_raises(AggError):
        canvas.draw_shape(path, transform, paint, paint, gs)


def test_clear():
    expected = np.zeros((4, 4, 3), dtype=np.uint8)
    buffer = np.zeros((4, 4, 3), dtype=np.uint8)
    canvas = CanvasRGB24(buffer)

    expected[:] = 0
    canvas.clear(0, 0, 0)
    assert_equal(expected, canvas.array)

    expected[:] = 128
    canvas.clear(.5, .5, .5)
    assert_equal(expected, canvas.array)

    expected[:] = 255
    canvas.clear(1, 1, 1)
    assert_equal(expected, canvas.array)


def test_line():
    expected = np.zeros((100, 100, 3), dtype=np.uint8)
    buffer = np.zeros((100, 100, 3), dtype=np.uint8)
    canvas = CanvasRGB24(buffer)
    path = Path()
    transform = Transform()
    line_paint = SolidPaint(1.0, 1.0, 1.0)
    gs = GraphicsState(anti_aliased=False, line_width=1.0)

    expected[1, ...] = 255
    path.move_to(0, 1.5)
    path.line_to(100, 1.5)
    canvas.draw_shape(path, transform, line_paint, line_paint, gs)
    assert_equal(expected, canvas.array)

    buffer[:] = 0
    expected[:] = 0

    expected[:, 1, :] = 255
    path.reset()
    path.move_to(1.5, 0)
    path.line_to(1.5, 100)
    canvas.draw_shape(path, transform, line_paint, line_paint, gs)
    assert_equal(expected, canvas.array)
