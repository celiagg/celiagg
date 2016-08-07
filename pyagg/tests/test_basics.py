from pyagg import Color, Rect


def test_color():
    c = Color(255, 255, 255, 255)
    assert (repr(c) == 'Color(255, 255, 255, 255)')

    c = Color(0, 0, 0, 0)
    assert c.invisible


def test_rect():
    r = Rect(0, 0, -1, -1)
    assert not r.valid
