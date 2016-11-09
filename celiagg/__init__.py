# The MIT License (MIT)
#
# Copyright (c) 2014-2016 WUSTL ZPLAB
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
# Authors: Erik Hvatum <ice.rikh@gmail.com>
#          John Wiggins <john.wiggins@xfel.eu>

# flake8: noqa
from __future__ import absolute_import

from ._celiagg import HAS_TEXT
from ._celiagg import (
    AggError, BlendMode, BSpline, DrawingMode, Font, FontCacheType,
    GradientSpread, GradientUnits, GraphicsState, Image, InnerJoin,
    LinearGradientPaint, LineCap, LineJoin, RadialGradientPaint, Path,
    PatternPaint, PatternStyle, PixelFormat, Rect, ShapeAtPoints, SolidPaint,
    TextDrawingMode, Transform
)
from ._celiagg import (CanvasG8, CanvasGA16, CanvasRGB24, CanvasRGBA32,
                     CanvasBGRA32, CanvasRGBA128)
