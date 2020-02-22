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
