About celiagg
=============

celiagg is designed to be a *simple*, *stateless* library for 2D vector graphics.
**Simple**, because it provides a general API which should be simple enough to
fit in ones head while still being a general solution for 2D vector
rasterization. **Stateless**, because unlike `Cairo <https://cairographics.org/>`_
or
`Quartz <https://developer.apple.com/library/content/documentation/GraphicsImaging/Conceptual/drawingwithquartz2d/dq_overview/dq_overview.html>`_
(for example), celiagg's main drawing object (``Canvas``) holds the absolute
minimum state required to do its job. Instead, the celiagg API includes a
handful of stateful objects which are used to pass various drawing attributes
to the methods of ``Canvas``. In this way, it is more like
`Skia <https://skia.org/user/api>`_ (but much simpler, of course).

High-level Overview
~~~~~~~~~~~~~~~~~~~

These are the main objects used for drawing in celiagg.

* ``Canvas``: The object which has the main drawing methods. It also manages
  the frame buffer which is being drawn into.
* :class:`celiagg.GraphicsState`: The main state container to be passed to all
  ``Canvas`` drawing methods.
* ``VertexSource``: A family of classes which contain vector data to be
  rasterized. Includes :class:`celiagg.Path`, :class:`celiagg.BSpline`, and
  :class:`celiagg.ShapeAtPoints`
* ``Paint``: Another family of classes which describe the color and texture of
  lines and fills. Includes :class:`celiagg.SolidPaint`,
  :class:`celiagg.PatternPaint`, :class:`celiagg.LinearGradientPaint`, and
  :class:`celiagg.RadialGradientPaint`.
* :class:`celiagg.Transform`: A 2D affine transform
* :class:`celiagg.Font`: A single font, needed when rendering text.
* :class:`celiagg.FontCache`: Needed to measure the size of rendered text.
* :class:`celiagg.Image`: A container for raster image data.
