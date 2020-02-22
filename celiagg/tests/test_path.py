import unittest

import numpy as np

from celiagg import Path, ShapeAtPoints


class TestPath(unittest.TestCase):
    def test_sap_bad_init_args(self):
        with self.assertRaises(TypeError):
            ShapeAtPoints(None, [(0, 0), (10, 10)])

    def test_move_to(self):
        pth = Path()
        pth.move_to(10, 10)
        pth.move_to(0, 0)

        self.assertTrue(np.all(pth.vertices() == [[10.0, 10.0], [0.0, 0.0]]))

    def test_copy(self):
        pth = Path()
        pth.move_to(10, 10)
        pth.move_to(0, 0)

        cpy = pth.copy()
        self.assertNotEqual(id(cpy), id(pth))
        self.assertEqual(len(cpy.vertices()), len(pth.vertices()))

        cpy_pts = np.array(list(iter(cpy)))
        pth_pts = np.array(list(iter(pth)))
        self.assertTrue(np.all(cpy_pts == pth_pts))

    def test_reset(self):
        pth = Path()
        pth.move_to(10, 10)
        pth.move_to(0, 0)

        pth.reset()
        self.assertEqual(len(pth.vertices()), 0)

    def test_closing(self):
        pth = Path()
        pth.move_to(10, 10)
        pth.move_to(0, 0)
        pth.close()

        self.assertEqual(len(pth.vertices()), 3)
        self.assertTrue(np.all(pth.vertices()[-1] == [0.0, 0.0]))

    def test_iteration(self):
        pth = Path()
        pth.move_to(10, 10)

        lp = list(iter(pth))
        self.assertEqual(len(lp), 1)
        self.assertTrue(np.all(lp[0] == [10.0, 10.0]))
