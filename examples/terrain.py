from __future__ import division
import argparse

import numpy as np
from skimage.io import imsave

import celiagg as agg


def draw_horizon(xs, ys, canvas, gs):
    def rcolor():
        return hsv2rgb(.125 + randneghalf2half() * .25,
                       .6 + randneghalf2half() * .4,
                       .6 + randneghalf2half() * .4)

    width = xs[-1]
    path = agg.Path()
    path.move_to(0, 0)
    path.line_to(xs[0], ys[0])
    for x, y in zip(xs[1:], ys[1:]):
        path.line_to(x, y)
    path.line_to(width, 0)
    path.close()

    transform = agg.Transform()
    fill = agg.LinearGradientPaint(0, 0, width, 0, randgradstops(2, rcolor),
                                   agg.GradientSpread.SpreadReflect,
                                   agg.GradientUnits.UserSpace)
    canvas.draw_shape(path, transform, gs, fill=fill)


def gen_horizon(numpoints, xmax, ymax):
    xs = np.linspace(0, xmax, numpoints, endpoint=True)
    ys = np.zeros(numpoints)
    ys[0] = np.random.rand(1) * ymax
    ys[-1] = np.random.rand(1) * ymax
    midpoint_displace(xs, ys, 0, numpoints-1)
    return xs, ys


def hsv2rgb(h, s, v, a=1.0):
    hp = h * 6
    c = s * v
    x = c * (1 - abs(hp % 2 - 1))
    if 0. <= hp <= 1.:
        return c, x, 0., a
    elif 1. < hp <= 2.:
        return x, c, 0, a
    elif 2. < hp <= 3.:
        return 0, c, x, a
    elif 3. < hp <= 4.:
        return 0, x, c, a
    elif 4. < hp <= 5.:
        return x, 0, c, a
    elif 5. < hp <= 6.:
        return c, 0, x, a
    return 0, 0, 0, a


def midpoint_displace(xs, ys, index0, index1):
    indexdist = index1 - index0
    if indexdist <= 1:
        return

    midindex = (index0 + index1) // 2
    xdist = xs[index1] - xs[index0]
    displace = randneghalf2half() * xdist
    ys[midindex] = (ys[index0] + ys[index1]) / 2 + displace

    midpoint_displace(xs, ys, index0, midindex)
    midpoint_displace(xs, ys, midindex, index1)


def nearest_pow2(value):
    count = 0
    while value > 0:
        count += 1
        value >>= 1
    return 2**count


def randgradstops(count, cfunc):
    return [(off,) + cfunc() for off in np.linspace(0, 1, count)]


def randneghalf2half():
    return (0.5 - np.random.rand(1)[0])


def sky(width, height, canvas, gs):
    def rcolor():
        return hsv2rgb(.55 + randneghalf2half() * .25,
                       .6 + randneghalf2half() * .125,
                       .25 + randneghalf2half() * .125)

    transform = agg.Transform()
    starpaint = agg.SolidPaint(*hsv2rgb(.165, .96, .99))
    starshape = agg.Path()
    angles = np.linspace(0, 2*np.pi, num=5, endpoint=False)
    pts = np.stack([np.cos(angles), np.sin(angles)]).T * 7
    starshape.lines([pts[i] for i in (0, 2, 4, 1, 3)])
    starshape.close()

    bgshape = agg.Path()
    bgshape.rect(0, 0, width, height)
    bgpaint = agg.LinearGradientPaint(0, 0, 0, height,
                                      randgradstops(2, rcolor),
                                      agg.GradientSpread.SpreadReflect,
                                      agg.GradientUnits.UserSpace)
    canvas.draw_shape(bgshape, transform, gs, fill=bgpaint)

    star_count = 40
    xs = np.random.rand(star_count) * width
    ys = np.random.rand(star_count) * height
    for x, y in zip(xs, ys):
        transform.reset()
        transform.translate(x, y)
        canvas.draw_shape(starshape, transform, gs, fill=starpaint)


def terrain(size):
    canvas = agg.CanvasRGB24(np.empty((size[1], size[0], 3), dtype='uint8'),
                             bottom_up=True)
    gs = agg.GraphicsState(drawing_mode=agg.DrawingMode.DrawFill)

    sky(size[0], size[1], canvas, gs)
    xs, ys = gen_horizon(nearest_pow2(size[0] // 4) + 1, size[0], size[1])
    draw_horizon(xs, ys / 2, canvas, gs)

    imsave('terrain.png', canvas.array)


if __name__ == '__main__':
    p = argparse.ArgumentParser()
    p.add_argument('-s', '--size', nargs=2, default=[1000, 1000])
    args = p.parse_args()
    size = tuple(int(a) for a in args.size)
    terrain(size)
