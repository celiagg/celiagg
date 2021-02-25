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

import celiagg as agg


@unittest.skipIf(agg.HAS_TEXT, 'Text support is available')
class TestNoTextSupport(unittest.TestCase):
    def test_no_text_draw_text_failure(self):
        buffer = np.zeros((1, 1), dtype=np.uint8)
        canvas = agg.CanvasG8(buffer)
        transform = agg.Transform()
        line_paint = agg.SolidPaint(1.0, 1.0, 1.0)
        gs = agg.GraphicsState()

        with self.assertRaises(RuntimeError):
            canvas.draw_text('', None, transform, line_paint, line_paint, gs)
