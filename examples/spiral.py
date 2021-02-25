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
import argparse

import numpy as np
from skimage.color import hsv2rgb
from skimage.io import imsave

import celiagg as agg

# Some constants to fiddle with
CENTER_RADIUS = 25
CIRCLE_COUNT = 60
CIRCLE_SIZE = 20


def compute_offsets(maxoffset):
    offset = CENTER_RADIUS
    radius = 0
    offsets = [offset]
    while offset < maxoffset:
        radius = np.pi * offset / CIRCLE_COUNT

        # Estimate how far to push out the next line
        newradius = np.pi * (offset + radius * 2) / CIRCLE_COUNT
        offset += radius + newradius
        offsets.append(offset)
    return offsets


def spiral(size, hue, sat, val):
    canvas = agg.CanvasRGB24(np.zeros((size[1], size[0], 3), dtype=np.uint8))
    gs = agg.GraphicsState(drawing_mode=agg.DrawingMode.DrawFill)
    transform = agg.Transform()
    circle = agg.Path()
    circle.ellipse(0, 0, CIRCLE_SIZE, CIRCLE_SIZE)

    divisions = np.linspace(0, 2*np.pi, CIRCLE_COUNT, endpoint=False)
    centers = np.stack((np.cos(divisions), np.sin(divisions)), axis=1)
    offsets = compute_offsets(np.sqrt(size[0]**2 + size[1]**2) / 2)
    color_count = len(offsets)

    hsv = np.ones((color_count, 1, 3))
    hsv[:, 0, 0] = np.linspace(hue[0], hue[1], color_count, endpoint=False)
    hsv[:, 0, 1] = np.linspace(sat[0], sat[1], color_count, endpoint=False)
    hsv[:, 0, 2] = np.linspace(val[0], val[1], color_count, endpoint=False)
    spectrum = hsv2rgb(hsv).reshape(color_count, 3)

    for idx, offset in enumerate(offsets):
        paint = agg.SolidPaint(*spectrum[idx])
        radius = np.pi * offset / CIRCLE_COUNT
        scale = radius / CIRCLE_SIZE
        for i in range(CIRCLE_COUNT):
            if ((idx + i) % 2) == 0:
                continue
            transform.reset()
            transform.translate(size[0]/2 + offset*centers[i, 0],
                                size[1]/2 + offset*centers[i, 1])
            transform.scale(scale, scale)
            canvas.draw_shape(circle, transform, gs, fill=paint)

    imsave('spiral.png', canvas.array)


if __name__ == '__main__':
    p = argparse.ArgumentParser()
    p.add_argument('-z', '--size', nargs=2, default=[1000, 1000])
    p.add_argument('-u', '--hue', nargs=2, default=[0.0, 1.0])
    p.add_argument('-s', '--sat', nargs=2, default=[0.95, 0.95])
    p.add_argument('-v', '--val', nargs=2, default=[0.85, 0.85])
    args = p.parse_args()
    size = tuple(int(a) for a in args.size)
    hue = tuple(float(a) for a in args.hue)
    sat = tuple(float(a) for a in args.sat)
    val = tuple(float(a) for a in args.val)
    spiral(size, hue, sat, val)
