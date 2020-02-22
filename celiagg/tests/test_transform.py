import unittest

import numpy as np
from numpy.testing import assert_allclose

from celiagg import Transform


class TestTransform(unittest.TestCase):
    def test_construction(self):
        t = Transform(1.0, 2.0, 3.0, 4.0, 5.0, 6.0)
        self.assertEqual(repr(t), 'Transform(1.0, 2.0, 3.0, 4.0, 5.0, 6.0)')

    def test_copy(self):
        t = Transform(1.0, 2.0, 3.0, 4.0, 5.0, 6.0)
        cpy = t.copy()
        self.assertEqual(repr(t), 'Transform(1.0, 2.0, 3.0, 4.0, 5.0, 6.0)')

    def test_scale(self):
        t = Transform()

        t.scale(2.0, 2.0)
        pt = t.worldToScreen(1.0, 1.0)
        self.assertEqual(pt, (2.0, 2.0))

        t.reset()
        t.scale(1.0, 2.0)
        pt = t.worldToScreen(1.0, 1.0)
        self.assertEqual(pt, (1.0, 2.0))

        t.reset()
        t.scale(2.0, 1.0)
        pt = t.worldToScreen(1.0, 1.0)
        self.assertEqual(pt, (2.0, 1.0))

    def test_rotate(self):
        t = Transform()

        t.rotate(np.pi / 2.0)
        pt = t.worldToScreen(1.0, 0.0)
        assert_allclose(pt, (0.0, 1.0), atol=1e-9)

        t.reset()
        t.rotate(np.pi)
        pt = t.worldToScreen(1.0, 0.0)
        assert_allclose(pt, (-1.0, 0.0), atol=1e-9)

        t.reset()
        t.rotate(-np.pi / 2.0)
        pt = t.worldToScreen(1.0, 0.0)
        assert_allclose(pt, (0.0, -1.0), atol=1e-9)

        t.reset()
        t.rotate(np.pi / 2.0)
        t.rotate(np.pi / 2.0)
        pt = t.worldToScreen(1.0, 0.0)
        assert_allclose(pt, (-1.0, 0.0), atol=1e-9)

    def test_translate(self):
        t = Transform()

        t.translate(10.0, 10.0)
        pt = t.worldToScreen(0.0, 0.0)
        self.assertEqual(pt, (10.0, 10.0))

        t.reset()
        t.translate(-10.0, -10.0)
        pt = t.worldToScreen(0.0, 0.0)
        self.assertEqual(pt, (-10.0, -10.0))

        t.reset()
        t.translate(10.0, 0.0)
        t.translate(0.0, 10.0)
        pt = t.worldToScreen(0.0, 0.0)
        self.assertEqual(pt, (10.0, 10.0))

    def test_roundtrip(self):
        t = Transform()

        t.scale(2.0, 2.0)
        t.rotate(np.pi)
        t.translate(5.0, 5.0)

        pt = t.screenToWorld(*t.worldToScreen(0.0, 0.0))
        assert_allclose(pt, (0.0, 0.0))
        pt = t.screenToWorld(*t.worldToScreen(42.0, 100.0))
        assert_allclose(pt, (42.0, 100.0))
