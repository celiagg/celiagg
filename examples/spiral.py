from __future__ import division
import argparse

import numpy as np
from skimage.color import hsv2rgb
from skimage.io import imsave

import celiagg as agg

# Some constants to fiddle with
CENTER_RADIUS = 25
CIRCLE_COUNT = 60
CIRCLE_SIZE = 20


def compute_offsets(maxdim):
    offset = CENTER_RADIUS
    radius = 0
    offsets = [offset]
    while offset - radius <= maxdim / 2:
        radius = np.pi * offset / CIRCLE_COUNT

        # Estimate how far to push out the next line
        newradius = np.pi * (offset + radius * 2) / CIRCLE_COUNT
        offset += radius + newradius
        offsets.append(offset)
    return offsets


def spiral(size):
    canvas = agg.CanvasRGB24(np.zeros((size[1], size[0], 3), dtype=np.uint8))
    gs = agg.GraphicsState(drawing_mode=agg.DrawingMode.DrawFill)
    transform = agg.Transform()
    circle = agg.Path()
    circle.ellipse(0, 0, CIRCLE_SIZE, CIRCLE_SIZE)

    offsets = compute_offsets(max(size))
    color_count = len(offsets)
    divisions = np.linspace(0, 2*np.pi, CIRCLE_COUNT, endpoint=False)
    centers = np.stack((np.cos(divisions), np.sin(divisions)), axis=1)

    hsv = np.ones((color_count, 1, 3))
    hsv[:, 0, 0] = np.linspace(0, 1, color_count, endpoint=False)
    hsv[:, 0, 1] *= 0.95  # Saturation
    hsv[:, 0, 2] *= 0.85  # Value
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
    p.add_argument('-s', '--size', nargs=2, default=[1000, 1000])
    args = p.parse_args()
    size = tuple(int(a) for a in args.size)
    spiral(size)
