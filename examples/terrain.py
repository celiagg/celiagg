from __future__ import division
import argparse

import numpy as np
from skimage.io import imsave

import celiagg as agg


def draw_horizon(xs, ys, canvas, gs, fill):
    path = agg.Path()
    path.move_to(0, 0)
    path.line_to(xs[0], ys[0])
    for x, y in zip(xs[1:], ys[1:]):
        path.line_to(x, y)
    path.line_to(size[0], 0)
    path.close()

    transform = agg.Transform()
    canvas.draw_shape(path, transform, gs, fill=fill)


def gen_horizon(numpoints, edgemax):
    xs = np.linspace(0, size[0], numpoints, endpoint=True)
    ys = np.zeros(numpoints)
    ys[0] = np.random.rand(1) * edgemax
    ys[-1] = np.random.rand(1) * edgemax
    midpoint_displace(xs, ys, 0, numpoints-1)
    return xs, ys


def hsv2rgb(h, s, v):
    hp = h * 6
    c = s * v
    x = c * (1 - abs(hp % 2 - 1))
    if 0. <= hp <= 1.:
        return c, x, 0.
    elif 1. < hp <= 2.:
        return x, c, 0
    elif 2. < hp <= 3.:
        return 0, c, x
    elif 3. < hp <= 4.:
        return 0, x, c
    elif 4. < hp <= 5.:
        return x, 0, c
    elif 5. < hp <= 6.:
        return c, 0, x
    return 0, 0, 0


def midpoint_displace(xs, ys, index0, index1):
    indexdist = index1 - index0
    if indexdist <= 1:
        return

    midindex = (index0 + index1) // 2
    xdist = xs[index1] - xs[index0]
    displace = (0.5 - np.random.rand(1)) * xdist
    ys[midindex] = (ys[index0] + ys[index1]) / 2 + displace

    midpoint_displace(xs, ys, index0, midindex)
    midpoint_displace(xs, ys, midindex, index1)


def nearest_pow2(value):
    count = 0
    while value > 0:
        count += 1
        value >>= 1
    return 2**count


def terrain(size):
    canvas = agg.CanvasRGB24(np.empty((size[1], size[0], 3), dtype='uint8'),
                             bottom_up=True)
    gs = agg.GraphicsState(drawing_mode=agg.DrawingMode.DrawFill)
    canvas.clear(0, 0, 0)

    numpoints = nearest_pow2(size[0] // 4) + 1
    for div in range(2, 9):
        xs, ys = gen_horizon(numpoints, size[1] / (div - 1))
        frac = (div - 1) / 8
        r, g, b = hsv2rgb(frac, .7, .7)
        fill = agg.SolidPaint(r, g, b, 0.5 * (1-frac) + 0.25)
        ys /= div
        draw_horizon(xs, ys, canvas, gs, fill)

    imsave('terrain.png', canvas.array)


if __name__ == '__main__':
    p = argparse.ArgumentParser()
    p.add_argument('-s', '--size', nargs=2, default=[1000, 1000])
    args = p.parse_args()
    size = tuple(int(a) for a in args.size)
    terrain(size)
