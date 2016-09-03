from pyagg import Rect


def test_rect():
    r = Rect(0, 0, -1, -1)
    assert not r.valid
