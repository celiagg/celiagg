import numpy as np
from skimage.io import imsave

import pyagg as agg

SIZE = 1000

order = (2, 4, 1, 3, 0)
angles = np.linspace(0, 2*np.pi, num=5, endpoint=False)
pts = np.stack([np.cos(angles), np.sin(angles)]).T * SIZE/2 + SIZE/2
star_shape = agg.Path()
star_shape.lines([pts[i] for i in (0, 2, 4, 1, 3)])
star_shape.close()
star_transform = agg.Transform()
star_transform.translate(SIZE/2, SIZE/2)
star_transform.rotate(-np.pi / 2)
star_transform.translate(-SIZE/2, -SIZE/2)

big_box = agg.Path()
big_box.rect(0, 0, SIZE, SIZE)
small_box = agg.Path()
small_box.rect(0, 0, SIZE/2, SIZE/2)

stencil_canvas = agg.CanvasG8(np.zeros((SIZE, SIZE), dtype=np.uint8))
canvas = agg.CanvasRGB24(np.zeros((SIZE, SIZE, 3), dtype=np.uint8))
gs = agg.GraphicsState(drawing_mode=agg.DrawingMode.DrawFill, line_width=6.0,
                       clip_box=agg.Rect(0, 0, SIZE, SIZE))
transform = agg.Transform()
blue_paint = agg.SolidPaint(0.1, 0.1, 1.0)
white_paint = agg.SolidPaint(1.0, 1.0, 1.0)
stops = ((0.0, 0.0, 0.0, 0.0, 1.0),
         (1.0, 0.3, 0.3, 0.75, 1.0))
bw_grad = agg.LinearGradientPaint(0, 0, 2, 5, stops,
                                  agg.GradientSpread.SpreadReflect,
                                  agg.GradientUnits.UserSpace)
stops = ((0.0, 1.0, 1.0, 1.0, 1.0),
         (1.0, 0.7, 0.7, 0.25, 1.0))
lin_grad = agg.LinearGradientPaint(0, 0, 2, 5, stops,
                                   agg.GradientSpread.SpreadReflect,
                                   agg.GradientUnits.UserSpace)

stencil_canvas.clear(0, 0, 0)
stencil_canvas.draw_shape(star_shape, star_transform, gs, fill=white_paint)

canvas.clear(1, 1, 1)
canvas.draw_shape(big_box, transform, gs, fill=bw_grad)
gs.stencil = stencil_canvas.image
canvas.draw_shape(star_shape, star_transform, gs, fill=lin_grad)

gs.drawing_mode = agg.DrawingMode.DrawStroke
gs.line_dash_pattern = ((5.0, 10.0), (10.0, 20.0))
transform.translate(SIZE/2, SIZE/2)
transform.scale(1.1, 1.1)
transform.rotate(np.pi / 4)
transform.translate(-SIZE/4, -SIZE/4)
canvas.draw_shape(small_box, transform, gs, stroke=blue_paint)
imsave('stencil.png', canvas.array)
