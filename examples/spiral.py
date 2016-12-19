from __future__ import division

import numpy as np
from skimage.color import hsv2rgb
from skimage.io import imsave

import celiagg as agg

SIZE = 1000
COUNT = 50
LEVELS = 35

canvas = agg.CanvasRGB24(np.zeros((SIZE, SIZE, 3), dtype=np.uint8))
gs = agg.GraphicsState(drawing_mode=agg.DrawingMode.DrawFill)
transform = agg.Transform()

canvas.clear(1, 1, 1)
circle = agg.Path()
circle.ellipse(0, 0, 50, 50)

ts = np.linspace(0, 2*np.pi, COUNT, endpoint=False)
centers = np.stack((np.cos(ts), np.sin(ts)), axis=1)

gray = agg.SolidPaint(0.5, 0.5, 0.5)
hsvs = np.ones((LEVELS, 1, 3))
hsvs[:, 0, 0] = np.linspace(0, 1, LEVELS, endpoint=False)
hsvs[:, 0, 1] *= 0.95  # Saturation
hsvs[:, 0, 2] *= 0.85  # Value
rgbs = hsv2rgb(hsvs).reshape(LEVELS, 3)

offset = 10
for d in range(LEVELS):
    radius = 2*np.pi*offset / (COUNT*2)
    scale = radius / 50
    colors = (gray, agg.SolidPaint(*rgbs[d]))

    for i in range(COUNT):
        transform.reset()
        transform.translate(SIZE/2 + offset*centers[i, 0],
                            SIZE/2 + offset*centers[i, 1])
        transform.scale(scale, scale)
        canvas.draw_shape(circle, transform, gs, fill=colors[(d+i) % 2])

    # Estimate how far to push out the next line
    newradius = 2 * np.pi * (offset + radius * 2) / (COUNT*2)
    offset += radius + newradius

imsave('spiral.png', canvas.array)
