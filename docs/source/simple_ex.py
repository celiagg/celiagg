import celiagg as agg
import numpy as np
from skimage.io import imsave

state = agg.GraphicsState(drawing_mode=agg.DrawingMode.DrawStroke,
                          line_width=10.0)
transform = agg.Transform()
red_paint = agg.SolidPaint(1.0, 0.0, 0.0)
black_paint = agg.SolidPaint(0.0, 0.0, 0.0)

canvas = agg.CanvasRGB24(np.ones((400, 400, 3), dtype=np.uint8))

path = agg.Path()
path.ellipse(200, 200, 190, 190)
canvas.clear(1.0, 1.0, 1.0)
canvas.draw_shape(path, transform, state, stroke=red_paint)

font = agg.Font("/Library/Fonts/Verdana.ttf", 96.0,
                agg.FontCacheType.RasterFontCache)
transform.translate(30.0, 220.0)
canvas.draw_text("celiagg", font, transform, state,
                 stroke=black_paint)

imsave("example.png", canvas.array)
