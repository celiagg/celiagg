import numpy as np
from skimage.io import imsave

import pyagg as agg


top = agg.Path()
top.move_to(60.510156, 6.3979729)
top.cubic_to(55.926503, 6.4192712, 51.549217, 6.8101906, 47.697656, 7.4917229)
top.cubic_to(36.35144, 9.4962267, 34.291407, 13.691825, 34.291406, 21.429223)
top.line_to(34.291406, 31.647973)
top.line_to(61.103906, 31.647973)
top.line_to(61.103906, 35.054223)
top.line_to(34.291406, 35.054223)
top.line_to(24.228906, 35.054223)
top.cubic_to(16.436447, 35.054223, 9.6131468, 39.73794, 7.4789058, 48.647973)
top.cubic_to(5.0170858, 58.860939, 4.9078907, 65.233996, 7.4789058, 75.897973)
top.cubic_to(9.3848341, 83.835825, 13.936449, 89.491721, 21.728906, 89.491723)
top.line_to(30.947656, 89.491723)
top.line_to(30.947656, 77.241723)
top.cubic_to(30.947656, 68.391821, 38.6048, 60.585475, 47.697656, 60.585473)
top.line_to(74.478906, 60.585473)
top.cubic_to(81.933857, 60.585473, 87.885159, 54.447309, 87.885156, 46.960473)
top.line_to(87.885156, 21.429223)
top.cubic_to(87.885156, 14.162884, 81.755176, 8.7044455, 74.478906, 7.4917229)
top.cubic_to(69.872919, 6.7249976, 65.093809, 6.3766746, 60.510156, 6.3979729)
top.close()
top.move_to(46.010156, 14.616723)
top.cubic_to(48.779703, 14.616723, 51.041406, 16.915369, 51.041406, 19.741723)
top.cubic_to(51.041404, 22.558059, 48.779703, 24.835473, 46.010156, 24.835473)
top.cubic_to(43.23068, 24.835472, 40.978906, 22.558058, 40.978906, 19.741723)
top.cubic_to(40.978905, 16.91537, 43.23068, 14.616723, 46.010156, 14.616723)
top.close()

bottom = agg.Path()
bottom.move_to(91.228906, 35.054223)
bottom.line_to(91.228906, 46.960473)
bottom.cubic_to(91.228906, 56.191228, 83.403011, 63.960472, 74.478906,
                63.960473)
bottom.line_to(47.697656, 63.960473)
bottom.cubic_to(40.361823, 63.960473, 34.291407, 70.238956, 34.291406,
                77.585473)
bottom.line_to(34.291406, 103.11672)
bottom.cubic_to(34.291406, 110.38306, 40.609994, 114.65704, 47.697656,
                116.74172)
bottom.cubic_to(56.184987, 119.23733, 64.323893, 119.68835, 74.478906,
                116.74172)
bottom.cubic_to(81.229061, 114.78733, 87.885159, 110.85411, 87.885156,
                103.11672)
bottom.line_to(87.885156, 92.897973)
bottom.line_to(61.103906, 92.897973)
bottom.line_to(61.103906, 89.491723)
bottom.line_to(87.885156, 89.491723)
bottom.line_to(101.29141, 89.491723)
bottom.cubic_to(109.08387, 89.491723, 111.98766, 84.056315, 114.69765,
                75.897973)
bottom.cubic_to(117.49698, 67.499087, 117.37787, 59.422197, 114.69765,
                48.647973)
bottom.cubic_to(112.77187, 40.890532, 109.09378, 35.054223, 101.29141,
                35.054223)
bottom.line_to(91.228906, 35.054223)
bottom.close()
bottom.move_to(76.166406, 99.710473)
bottom.cubic_to(78.945884, 99.710476, 81.197656, 101.98789, 81.197656,
                104.80422)
bottom.cubic_to(81.197654, 107.63057, 78.945881, 109.92922, 76.166406,
                109.92922)
bottom.cubic_to(73.396856, 109.92922, 71.135156, 107.63057, 71.135156,
                104.80422)
bottom.cubic_to(71.135158, 101.98789, 73.396853, 99.710473, 76.166406,
                99.710473)
bottom.close()

stops = ((0.0, 0.35294, 0.62353, 0.83137, 1.0),
         (1.0, 0.18824, 0.41176, 0.59608, 1.0))
top_grad = agg.LinearGradientPaint(
    26.648937, 20.603781, 135.66525, 114.39767, stops,
    agg.GradientSpread.SpreadPad, agg.GradientUnits.UserSpace)
top_grad.transform = agg.Transform(0.562541, 0, 0,
                                   0.567972, -9.399749, -5.305317)
stops = ((0.0, 1.0, 0.83137, 0.23137, 1.0),
         (1.0, 1.0, 0.90980, 0.45098, 1.0))
bottom_grad = agg.LinearGradientPaint(
    150.96111, 192.35176, 112.03144, 137.27299, stops,
    agg.GradientSpread.SpreadPad, agg.GradientUnits.UserSpace)
bottom_grad.transform = agg.Transform(0.562541, 0, 0,
                                      0.567972, -9.399749, -5.305317)

canvas = agg.CanvasRGB24(np.zeros((600, 600, 3), dtype=np.uint8))
gs = agg.GraphicsState(drawing_mode=agg.DrawingMode.DrawFill,
                       clip_box=agg.Rect(0, 0, 600, 600))
canvas.clear(1, 1, 1)
transform = agg.Transform()
transform.translate(300, 300)
transform.scale(5, 5)
transform.rotate(np.pi/4)
transform.translate(-60, -60)
canvas.draw_shape(top, transform, gs, fill=top_grad)
canvas.draw_shape(bottom, transform, gs, fill=bottom_grad)
imsave("python.png", canvas.array)
