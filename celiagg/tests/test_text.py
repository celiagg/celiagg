
import numpy as np

import celiagg as agg


def test_font_byte_string():
    canvas = agg.CanvasG8(np.zeros((1, 1), dtype=np.uint8))
    gs = agg.GraphicsState()
    transform = agg.Transform()

    text_unicode = u"Hello!"
    font_unicode = agg.Font(u"Times New Roman", 12.0, agg.FontCacheType.RasterFontCache)
    text_byte = b"Hello!"
    font_byte = agg.Font(b"Times New Roman", 12.0, agg.FontCacheType.RasterFontCache)

    canvas.draw_text(text_unicode, font_unicode, transform, gs)
    canvas.draw_text(text_byte, font_unicode, transform, gs)
    canvas.draw_text(text_unicode, font_byte, transform, gs)
    canvas.draw_text(text_byte, font_byte, transform, gs)
