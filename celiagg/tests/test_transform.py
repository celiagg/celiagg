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
from numpy.testing import assert_allclose

from celiagg import Transform


class TestTransform(unittest.TestCase):
    def test_construction(self):
        t = Transform(1.0, 2.0, 3.0, 4.0, 5.0, 6.0)
        self.assertEqual(repr(t), 'Transform(1.0, 2.0, 3.0, 4.0, 5.0, 6.0)')

    def test_copy(self):
        t = Transform(1.0, 2.0, 3.0, 4.0, 5.0, 6.0)
        cpy = t.copy()
        self.assertEqual(repr(cpy), 'Transform(1.0, 2.0, 3.0, 4.0, 5.0, 6.0)')

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
