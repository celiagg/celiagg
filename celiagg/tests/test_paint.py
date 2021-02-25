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

from celiagg import (GradientSpread, GradientUnits, SolidPaint,
                     LinearGradientPaint, RadialGradientPaint)

_GRADIENT_STOPS = ((0.0, 1.0, 0.0, 0.0, 1.0),
                   (0.5, 1.0, 1.0, 1.0, 1.0),
                   (1.0, 0.0, 0.0, 0.4, 1.0))


class TestPaint(unittest.TestCase):
    def test_solid_paint(self):
        paint0 = SolidPaint(0.0, 0.0, 0.0)
        paint1 = SolidPaint(0.0, 1.0, 1.0)

        # XXX: There is no `!=` operator for SolidPaint
        self.assertFalse(paint0 == paint1)
        self.assertEqual(paint0.copy(), SolidPaint(0, 0, 0))

    def test_linear_gradient_paint(self):
        paint = LinearGradientPaint(100, 100, 0, 0, _GRADIENT_STOPS,
                                    GradientSpread.SpreadRepeat,
                                    GradientUnits.ObjectBoundingBox)
        paint.copy()

    def test_radial_gradient_paint(self):
        paint = RadialGradientPaint(0, 0, 100, 200, 200, _GRADIENT_STOPS,
                                    GradientSpread.SpreadRepeat,
                                    GradientUnits.UserSpace)
        paint.copy()
