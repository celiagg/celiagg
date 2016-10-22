from pyagg import (GradientSpread, GradientUnits, SolidPaint,
                   LinearGradientPaint, RadialGradientPaint)

_GRADIENT_STOPS = ((0.0, 1.0, 0.0, 0.0, 1.0),
                   (0.5, 1.0, 1.0, 1.0, 1.0),
                   (1.0, 0.0, 0.0, 0.4, 1.0))


def test_solid_paint():
    paint0 = SolidPaint(0.0, 0.0, 0.0)
    paint1 = SolidPaint(0.0, 1.0, 1.0)

    assert not (paint0 == paint1)
    assert paint0.copy() == SolidPaint(0, 0, 0)


def test_linear_gradient_paint():
    paint = LinearGradientPaint(100, 100, 0, 0, _GRADIENT_STOPS,
                                GradientSpread.SpreadRepeat,
                                GradientUnits.ObjectBoundingBox)
    paint.copy()


def test_radial_gradient_paint():
    paint = RadialGradientPaint(0, 0, 100, 200, 200, _GRADIENT_STOPS,
                                GradientSpread.SpreadRepeat,
                                GradientUnits.UserSpace)
    paint.copy()
