import weakref

from nose.tools import assert_raises
import numpy as np
from numpy.testing import assert_equal

import celiagg as agg


def test_cleanup():
    # Make sure our array references don't linger
    buffer = np.zeros((10, 10, 3), dtype=np.uint8)
    canvas = agg.CanvasRGB24(buffer)

    weak_buf = weakref.ref(buffer)
    weak_arr = weakref.ref(canvas.array)

    assert weak_buf() is weak_arr()

    del buffer
    assert weak_buf() is not None

    del canvas
    assert weak_buf() is None


def test_bad_method_args():
    canvas = agg.CanvasG8(np.zeros((1, 1), dtype=np.uint8))
    pix_format = agg.PixelFormat.Gray8
    gs = agg.GraphicsState()
    path = agg.Path()
    transform = agg.Transform()

    with assert_raises(TypeError):
        canvas.draw_image(None, pix_format, transform, gs)
    with assert_raises(TypeError):
        canvas.draw_image(canvas.array, None, transform, gs)
    with assert_raises(TypeError):
        canvas.draw_image(canvas.array, pix_format, None, gs)
    with assert_raises(TypeError):
        canvas.draw_image(canvas.array, pix_format, transform, None)
    # But this version should work
    canvas.draw_image(canvas.image, None, transform, gs)

    with assert_raises(TypeError):
        canvas.draw_shape(None, transform, gs)
    with assert_raises(TypeError):
        canvas.draw_shape(path, None, gs)
    with assert_raises(TypeError):
        canvas.draw_shape(path, transform, None)

    text = "Hello!"
    font = agg.Font("Times New Roman", 12.0, agg.FontCacheType.RasterFontCache)
    with assert_raises(TypeError):
        canvas.draw_text(text, None, transform, gs)
    with assert_raises(TypeError):
        canvas.draw_text(text, font, None, gs)
    with assert_raises(TypeError):
        canvas.draw_text(text, font, transform, None)


def test_stencil_size_mismatch():
    canvas = agg.CanvasRGB24(np.zeros((4, 5, 3), dtype=np.uint8))
    stencil_canvas = agg.CanvasG8(np.zeros((1, 2), dtype=np.uint8))
    gs = agg.GraphicsState(stencil=stencil_canvas.image)
    path = agg.Path()
    transform = agg.Transform()

    with assert_raises(agg.AggError):
        canvas.draw_shape(path, transform, gs)


def test_clear():
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


def test_line():
    expected = np.zeros((10, 10), dtype=np.uint8)
    buffer = np.zeros((10, 10), dtype=np.uint8)
    canvas = agg.CanvasG8(buffer)
    path = agg.Path()
    transform = agg.Transform()
    paint = agg.SolidPaint(1.0, 1.0, 1.0)
    bounds = agg.Rect(0, 0, 10, 10)
    gs = agg.GraphicsState(anti_aliased=False, line_width=1.0, clip_box=bounds)

    expected[1, ...] = 255
    path.move_to(0, 1.5)
    path.line_to(10, 1.5)
    canvas.draw_shape(path, transform, gs, stroke=paint)
    assert_equal(expected, canvas.array)

    buffer[:] = 0
    expected[:] = 0

    expected[:, 1] = 255
    path.reset()
    path.move_to(1.5, 0)
    path.line_to(1.5, 10)
    canvas.draw_shape(path, transform, gs, stroke=paint)
    assert_equal(expected, canvas.array)
