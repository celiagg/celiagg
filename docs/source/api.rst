celiagg API documentation
=========================

.. automodule:: celiagg


Canvas Classes
--------------

There are many canvas classes which all have a common interface. The difference
between the classes is the color format of the destination frame buffer. The
list of available canvas classes follows this documentation of ``CanvasRGBA32``
which covers the interface:

.. autoclass:: CanvasRGBA32
   :members:
   :inherited-members:

.. autoclass:: CanvasG8

.. autoclass:: CanvasGA16

.. autoclass:: CanvasRGB24

.. autoclass:: CanvasBGRA32

.. autoclass:: CanvasRGBA128


Drawing State Container Classes
-------------------------------

There are a handful of objects which contain state or data useful to the
various drawing methods of ``Canvas``. ``GraphicsState`` and ``Transform`` for
example, are passed to every drawing method. ``Image`` instances can either be
drawn directly on a ``Canvas``, or supplied to ``PatternPaint`` to serve as a
repeating fill pattern. Finally, ``Font`` determines the font used when drawing
text, and can also be used to measure the bounding rectangle of a rendered
string.

.. autoclass:: GraphicsState

.. autoclass:: Image

.. autoclass:: Font
   :members:

.. autoclass:: Rect

.. autoclass:: Transform
   :members:


Paint Classes
-------------

The ``Paint`` (an internal base class) classes determine the look of strokes
and fills. ``Canvas.draw_shape`` and ``Canvas.draw_text`` both accept optional
arguments for `fill` and `stroke` ``Paint``s. The currently available options
are solid colors, gradients, or image patterns.

.. autoclass:: LinearGradientPaint

.. autoclass:: RadialGradientPaint

.. autoclass:: PatternPaint

.. autoclass:: SolidPaint


``VetexSource`` Classes
-----------------------

The ``VertexSource`` (an internal base class) classes contain geometry data
which can be drawn using the ``Canvas.draw_shape`` method.

.. autoclass:: BSpline
   :members:
   :inherited-members:

.. autoclass:: Path
   :members:
   :inherited-members:

.. autoclass:: ShapeAtPoints
   :members:
   :inherited-members:


Enumerations
------------

BlendMode
~~~~~~~~~

NOTE: The various blend modes are currently *ignored*.

  * ``BlendAlpha``
  * ``BlendClear``
  * ``BlendSrc``
  * ``BlendDst``
  * ``BlendSrcOver``
  * ``BlendDstOver``
  * ``BlendSrcIn``
  * ``BlendDstIn``
  * ``BlendSrcOut``
  * ``BlendDstOut``
  * ``BlendSrcAtop``
  * ``BlendDstAtop``
  * ``BlendXor``
  * ``BlendAdd``
  * ``BlendMultiply``
  * ``BlendScreen``
  * ``BlendOverlay``
  * ``BlendDarken``
  * ``BlendLighten``
  * ``BlendColorDodge``
  * ``BlendColorBurn``
  * ``BlendHardLight``
  * ``BlendSoftLight``
  * ``BlendDifference``
  * ``BlendExclusion``

DrawingMode
~~~~~~~~~~~

Drawing mode: outline (stroke) only, fill only, or a combination. The ``Eof``
fill variations use the even-odd rule when filling self-intersecting curves.

  * ``DrawFill``
  * ``DrawEofFill``
  * ``DrawStroke``
  * ``DrawFillStroke``
  * ``DrawEofFillStroke``

FontCacheType
~~~~~~~~~~~~~

Determines whether text is rendered with a raster or vector representation.
Raster text is faster, but doesn't support different stroke and fill paints;
only solid color. Vector text is slower but very powerful, and can make use of
gradients or patterned fills/strokes.

  * ``RasterFontCache``
  * ``VectorFontCache``

GradientSpread
~~~~~~~~~~~~~~

How a gradient behaves at its boundaries. ``Pad`` means that the colors at the
ends of a gradient extend to infinity. ``Reflect`` sweeps back and forth over
a gradient, and ``Repeat`` starts over at the first color after getting to the
end of a gradient.

  * ``SpreadPad``
  * ``SpreadReflect``
  * ``SpreadRepeat``

GradientUnits
~~~~~~~~~~~~~

How the coordinates of a gradient object are interpreted. ``UserSpace`` means
the local transform (the ``Transform`` passed to the draw method) affects the
gradient coordinates. ``ObjectBoundingBox`` means the coordinates are
interpreted as relative values (percentages) within the bounding box of the
``VertexSource`` object currently being drawn.

NOTE: This is identical to how SVG handles gradients.
See https://www.w3.org/TR/SVG/pservers.html for a more lucid description.

  * ``UserSpace``
  * ``ObjectBoundingBox``

InnerJoin
~~~~~~~~~

  * ``InnerBevel``
  * ``InnerMiter``
  * ``InnerJag``
  * ``InnerRound``

LineCap
~~~~~~~

  * ``CapButt``
  * ``CapSquare``
  * ``CapRound``

LineJoin
~~~~~~~~

  * ``JoinMiter``
  * ``JoinRound``
  * ``JoinBevel``

PatternStyle
~~~~~~~~~~~~

Similar to ``GradientSpread``, patterns can either reflect or repeat.

  * ``StyleRepeat``
  * ``StyleReflect``

PixelFormat
~~~~~~~~~~~

  * ``Gray8``
  * ``Gray16``
  * ``Gray32``
  * ``BGR24``
  * ``RGB24``
  * ``BGR48``
  * ``RGB48``
  * ``BGR96``
  * ``RGB96``
  * ``BGRA32``
  * ``RGBA32``
  * ``ARGB32``
  * ``ABGR32``
  * ``BGRA64``
  * ``RGBA64``
  * ``ARGB64``
  * ``ABGR64``
  * ``BGRA128``
  * ``RGBA128``
  * ``ARGB128``
  * ``ABGR128``

TextDrawingMode
~~~~~~~~~~~~~~~

  * ``TextDrawInvisible``
  * ``TextDrawFill``
  * ``TextDrawStroke``
  * ``TextDrawClip``
  * ``TextDrawFillStroke``
  * ``TextDrawFillClip``
  * ``TextDrawStrokeClip``
  * ``TextDrawFillStrokeClip``


Miscellaneous
~~~~~~~~~~~~~

.. autoexception:: AggError
