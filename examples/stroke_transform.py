from __future__ import division
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
