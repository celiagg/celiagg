# The MIT License (MIT)
#
# Copyright (c) 2016-2023 Celiagg Contributors
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
import argparse

import numpy as np
from skimage.io import imsave

import celiagg as agg


def draw_strokes(size):
    canvas = agg.CanvasRGB24(np.empty((size[1], size[0], 3), dtype='uint8'),
                             bottom_up=True)
    canvas.clear(1.0, 1.0, 1.0)

    gs = agg.GraphicsState(drawing_mode=agg.DrawingMode.DrawStroke)

    transform = agg.Transform()
    paint = agg.SolidPaint(1.0, 0.0, 0.0, 1.0)
    shape = agg.Path()
    angles = np.linspace(0, 2*np.pi, num=5, endpoint=False)
    pts = np.stack([np.cos(angles), np.sin(angles)]).T * 7
    shape.lines([pts[i] for i in (0, 2, 4, 1, 3)])
    shape.close()

    transform.translate(size[0] / 2, size[1] / 2)
    transform.scale(60, 10)
    canvas.draw_shape(shape, transform, gs, stroke=paint)

    imsave('strokes.png', canvas.array)


if __name__ == '__main__':
    p = argparse.ArgumentParser()
    p.add_argument('-s', '--size', nargs=2, default=[1000, 1000])
    args = p.parse_args()
    size = tuple(int(a) for a in args.size)
    draw_strokes(size)
