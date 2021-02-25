import celiagg as agg
import numpy as np
from PIL import Image

state = agg.GraphicsState(drawing_mode=agg.DrawingMode.DrawStroke,
                          line_width=10.0)
transform = agg.Transform()
red_paint = agg.SolidPaint(1.0, 0.0, 0.0)
orange_paint = agg.SolidPaint(0.99, 0.66, 0.0)

canvas = agg.CanvasRGB24(np.zeros((400, 400, 3), dtype=np.uint8))

path = agg.Path()
path.ellipse(200, 200, 190, 190)
canvas.clear(1.0, 1.0, 1.0)
canvas.draw_shape(path, transform, state, stroke=red_paint)

font = agg.Font(agg.example_font(), 96.0)
transform.translate(30.0, 220.0)
canvas.draw_text("celiagg", font, transform, state, fill=orange_paint)

image = Image.fromarray(canvas.array, "RGB")
image.save("example.png")
