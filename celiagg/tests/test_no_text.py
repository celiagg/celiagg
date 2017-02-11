import numpy as np
import pytest

import celiagg as agg


def test_no_text_font_failure():
    if agg.HAS_TEXT:
        return

    with pytest.raises(RuntimeError):
        agg.Font()


def test_no_text_draw_text_failure():
    if agg.HAS_TEXT:
        return

    buffer = np.zeros((1, 1), dtype=np.uint8)
    canvas = agg.CanvasG8(buffer)
    transform = agg.Transform()
    line_paint = agg.SolidPaint(1.0, 1.0, 1.0)
    gs = agg.GraphicsState()

    with pytest.raises(RuntimeError):
        canvas.draw_text("", None, transform, line_paint, line_paint, gs)
