Celiagg CHANGELOG
=================

Release 2.2.0
-------------

Features
~~~~~~~~

* Use Harfbuzz for shaping (#94)

Changes
~~~~~~~

* Return text cursor position after drawing (#104)

Cleanup and other maintenance
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

* Build wheels for a wider range of platforms using CIBuildWheel (#115)
* Add a minimal pyproject.yaml for isolated builds (#114)
* Test using non-EDM Python versions (#112)
* Update GitHub actions and wheel building (#107, #108, #110)

Release 2.1.3
-------------

Released: 2023-01-22

Changes
~~~~~~~

* Catch std::overflow_error exceptions from AGG (#97)

Cleanup and other maintenance
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

* Update GitHub actions to match main branch (#109, #110)

Release 2.1.1
-------------

Released: 2021-03-19

Changes
~~~~~~~

* Fix Path.add_path() (#92)

Release 2.1.0
-------------

Released: 2021-03-18

Features
~~~~~~~~

* Divide Font descriptions into FreeType and Win32 (#85)

Changes
~~~~~~~

* Support face names of arbitrary length in find_face (#89)
* Remember the face index passed into load_font (#84)

Documentation
~~~~~~~~~~~~~

* Update docs for 2.1 changes (#87)

Cleanup and other maintenance
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

* Update the source dependencies for linux wheels (#92)
* Merge setup.py into a single version (#91)

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
