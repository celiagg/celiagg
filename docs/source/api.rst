celiagg API documentation
=========================

.. automodule:: celiagg


Canvas Classes
--------------

.. autoclass:: CanvasG8
   :members:
   :inherited-members:

.. autoclass:: CanvasRGB24
   :members:
   :inherited-members:

.. autoclass:: CanvasRGBA32
   :members:
   :inherited-members:


Drawing State Container Classes
-------------------------------

.. autoclass:: GraphicsState

.. autoclass:: Image

.. autoclass:: Font
   :members:

.. autoclass:: Transform
   :members:


Paint Classes
-------------

.. autoclass:: LinearGradientPaint

.. autoclass:: RadialGradientPaint

.. autoclass:: PatternPaint

.. autoclass:: SolidPaint


``VetexSource`` Classes
-----------------------

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

  * ``DrawFill``
  * ``DrawEofFill``
  * ``DrawStroke``
  * ``DrawFillStroke``
  * ``DrawEofFillStroke``

FontCacheType
~~~~~~~~~~~~~

  * ``RasterFontCache``
  * ``VectorFontCache``

GradientSpread
~~~~~~~~~~~~~~

  * ``SpreadPad``
  * ``SpreadReflect``
  * ``SpreadRepeat``

GradientUnits
~~~~~~~~~~~~~

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
