Celiagg CHANGELOG
=================

Release 2.0.0
-------------

Released: 2021-02-25

Celiagg 2.0.0 is a major release which makes a few breaking API changes. Due
to bugs related to the font caching changes introduced in 1.1.1, a new
``FontCache`` class has been added which exposes the storage of the cache
explicitly. Relatedly, the ``Font`` class has become much simpler. It is only
a description for a font now and has no opinion about how that font is cached.
This is the main API breaking change. The ``FontCacheType`` enumeration has been
removed and the ``Font.width` method has moved to the new ``FontCache`` class.
One other change to ``Font`` behavior is that raster font rendering now uses the
``fill`` paint which is passed to ``Canvas.draw_text``. Previously the ``stroke``
paint was used.

A ``Canvas.draw_shape_at_points`` method was added to the canvas class. This
has a slightly different result than using a ``ShapeAtPoints`` ``VertexSource``.
Namely ``Canvas.draw_shape_at_points`` will fill and stroke each shape separately,
whereas ``ShapeAtPoints`` would fill all shape instances and then stroke all shape
instances. Use ``ShapeAtPoints`` if you're only stroking or only filling the
shape, because it's the faster option. ``Canvas.draw_shape_at_points`` is slightly
slower but gives more correct output.

Features
~~~~~~~~

* Add a ``draw_shape_at_points`` method to the canvas (#79)

Changes
~~~~~~~

* Rework text rendering (#76)
* Make the font cache a separate object [introduces ``FontCache``] (#74)

Documentation
~~~~~~~~~~~~~

* Update docs for 2.0.0 release (#80)
* Move the included font into the main package and update docs (#77)

Cleanup and other maintenance
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

* Update version and copyright dates (#78)


Release 1.1.1
-------------

Released: 2021-02-20

Features
~~~~~~~~

* Add ``face_index`` support to ``Font`` (#70)

Fixes
~~~~~

* Do stroke transformation as late as possible (#67)
* Speed up text rendering (#65)
* Use nearest neighbor image interpolation (#61)

Cleanup and other maintenance
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

* Clean up old CI bits (#69)
* Switch to GitHub Actions for CI (#68)
* Move CI testing to Pythons 3.6, 3.7 (#66)
* Use stdlib unittest instead of pytest (#64)
