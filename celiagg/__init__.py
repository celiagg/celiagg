# The MIT License (MIT)
#
# Copyright (c) 2014-2016 WUSTL ZPLAB
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
# Authors: Erik Hvatum <ice.rikh@gmail.com>
#          John Wiggins <john.wiggins@xfel.eu>

from . import _celiagg
from ._celiagg import (
    AggError, BSpline, BlendMode, DrawingMode, Font, FontCache, FontCacheType,
    GradientSpread, GradientUnits, GraphicsState, Image, InnerJoin,
    LineCap, LineJoin, LinearGradientPaint, Path, PatternPaint, PatternStyle,
    PixelFormat, RadialGradientPaint, Rect, ShapeAtPoints, SolidPaint,
    TextDrawingMode, Transform,
)
from ._celiagg import HAS_TEXT

# Be explicit
__all__ = [
    'HAS_TEXT',

    'AggError', 'BlendMode', 'BSpline', 'DrawingMode', 'Font', 'FontCache',
    'FontCacheType', 'GradientSpread', 'GradientUnits', 'GraphicsState',
    'Image', 'InnerJoin', 'LinearGradientPaint', 'LineCap', 'LineJoin',
    'RadialGradientPaint', 'Path', 'PatternPaint', 'PatternStyle',
    'PixelFormat', 'Rect', 'ShapeAtPoints', 'SolidPaint', 'TextDrawingMode',
    'Transform',

    'CanvasG8', 'CanvasGA16', 'CanvasRGB24', 'CanvasRGBA32', 'CanvasBGRA32',
    'CanvasRGBA128',
]

# Keep a font cache for callers that don't want to mess with it
__global_font_cache = None

_canvas_doc_string = """{klass_name}(array, bottom_up=False, font_cache=None)
Provides AGG (Anti-Grain Geometry) drawing routines that render to the
numpy array passed as the constructor argument. Because this array is
modified in place, it must be of type ``{array_type}``, must be
C-contiguous, and must be {channel_desc}.

:param array: A ``{array_type}`` array with shape {array_shape}.
:param bottom_up: If True, the origin is the bottom left, instead of top-left
:param font_cache: A ``FontCache`` instance. Defaults to a global instance.
"""


def _use_global_cache():
    """ Return the global ``FontCache`` instance.
    """
    global __global_font_cache
    if __global_font_cache is None:
        __global_font_cache = FontCache()
    return __global_font_cache


def _build_canvas_factory(klass_name, array_type, array_shape, channel_desc):
    """ Generate a Canvas factory.

    This is done to preserve the v1.0.0 interface of canvas class constructors.
    Using these factory functions keeps the ``font_cache`` parameter optional.
    """
    klass = getattr(_celiagg, klass_name)

    def factory(array, bottom_up=False, font_cache=None):
        cache = _use_global_cache() if font_cache is None else font_cache
        return klass(array, cache, bottom_up=bottom_up)

    factory.__doc__ = _canvas_doc_string.format(
        klass_name=klass_name,
        array_type=array_type,
        array_shape=array_shape,
        channel_desc=channel_desc,
    )
    factory.__name__ = klass_name
    return factory


# Generate the canvas classes
CanvasG8 = _build_canvas_factory(
    'CanvasG8',
    'numpy.uint8',
    '(H, W)',
    'MxN (1 channel: intensity)',
)
CanvasGA16 = _build_canvas_factory(
    'CanvasGA16',
    'numpy.uint8',
    '(H, W, 2)',
    'MxNx2 (2 channels: intensity and alpha)',
)
CanvasRGB24 = _build_canvas_factory(
    'CanvasRGB24',
    'numpy.uint8',
    '(H, W, 3',
    'MxNx3 (3 channels: red, green, and blue)',
)
CanvasRGBA32 = _build_canvas_factory(
    'CanvasRGBA32',
    'numpy.uint8',
    '(H, W, 4)',
    'MxNx4 (4 channels: red, green, blue, and alpha)',
)
CanvasBGRA32 = _build_canvas_factory(
    'CanvasBGRA32',
    'numpy.uint8',
    '(H, W, 4)',
    'MxNx4 (4 channels: blue, green, red, and alpha)',
)
CanvasRGBA128 = _build_canvas_factory(
    'CanvasRGBA128',
    'numpy.float32',
    '(H, W, 4)',
    'MxNx4 (2 channels: red, green, blue, and alpha)',
)
