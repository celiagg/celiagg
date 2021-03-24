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
import math

import numpy as np
from PIL import Image

import celiagg as agg

SIZE = 1000
HALF = SIZE / 2
QUARTER = HALF / 2
IMG_X = 50
IMG_Y = 50
image = np.random.randint(0, 256, size=(IMG_Y, IMG_X, 4), dtype=np.uint8)
image[:, :, 3] = 255

box = agg.Path()
box.rect(0, 0, HALF, HALF)

canvas = agg.CanvasRGB24(np.zeros((SIZE, SIZE, 3), dtype=np.uint8))
gs = agg.GraphicsState(drawing_mode=agg.DrawingMode.DrawFill)
transform = agg.Transform()
blue_paint = agg.SolidPaint(0.1, 0.1, 1.0)

canvas.clear(1, 1, 1)
canvas.draw_shape(box, transform, gs, fill=blue_paint)

gs.master_alpha = 0.5
positions = (
    (HALF - IMG_X/2, QUARTER - IMG_Y/2,
     math.pi / 3, agg.InterpolationMode.Bilinear),
    (HALF - IMG_X/2, HALF - IMG_Y/2,
     math.pi / 4, agg.InterpolationMode.Bicubic),
    (QUARTER - IMG_X/2, HALF - IMG_Y/2,
     math.pi / 6, agg.InterpolationMode.Sinc64),
)
for (x, y, rot, interp) in positions:
    transform.reset()
    transform.translate(-IMG_X, -IMG_Y)
    transform.scale(4, 4)
    transform.translate(x/4, y/4)
    transform.rotate(rot)
    gs.image_interpolation_mode = interp
    canvas.draw_image(image, agg.PixelFormat.RGBA32, transform, gs)

Image.fromarray(canvas.array).save('image.png')
