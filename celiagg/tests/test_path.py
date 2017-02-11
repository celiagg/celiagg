import numpy as np
import pytest

from celiagg import Path, ShapeAtPoints


def test_sap_bad_init_args():
    with pytest.raises(TypeError):
        ShapeAtPoints(None, [(0, 0), (10, 10)])


def test_move_to():
    pth = Path()
    pth.move_to(10, 10)
    pth.move_to(0, 0)

    assert np.all(pth.vertices() == [[10.0, 10.0], [0.0, 0.0]])


def test_copy():
    pth = Path()
    pth.move_to(10, 10)
    pth.move_to(0, 0)

    cpy = pth.copy()
    assert id(cpy) != id(pth)
    assert len(cpy.vertices()) == len(pth.vertices())

    cpy_pts = np.array(list(iter(cpy)))
    pth_pts = np.array(list(iter(pth)))
    assert np.all(cpy_pts == pth_pts)


def test_reset():
    pth = Path()
    pth.move_to(10, 10)
    pth.move_to(0, 0)

    pth.reset()
    assert len(pth.vertices()) == 0


def test_closing():
    pth = Path()
    pth.move_to(10, 10)
    pth.move_to(0, 0)
    pth.close()

    assert len(pth.vertices()) == 3
    assert np.all(pth.vertices()[-1] == [0.0, 0.0])


def test_iteration():
    pth = Path()
    pth.move_to(10, 10)

    lp = list(iter(pth))
    assert len(lp) == 1
    assert np.all(lp[0] == [10.0, 10.0])
