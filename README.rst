celiagg
=======

.. image:: https://img.shields.io/pypi/v/celiagg.svg
  :target: https://pypi.python.org/pypi/celiagg
  :alt: PyPI

.. image:: https://github.com/celiagg/celiagg/workflows/Test/badge.svg
   :target: https://github.com/celiagg/celiagg/actions?query=workflow%3A%22Test%22
   :alt: Build status

celiagg provides a simple, stateless canvas object that uses
Anti-Grain Geometry 2.4 <svn://svn.code.sf.net/p/agg/svn> with Cython to
render directly into a Numpy array.

A respectably modern C++ compiler is required to build celiagg

`Documentation! <https://celiagg.github.io/celiagg/>`_

Installation
------------

``pip install celiagg`` should do the trick.

Building from source with the Freetype font library on macOS requires
the `pkg-config` tool which can be installed via Homebrew, MacPorts, or
other macOS package management systems.

Dependencies
------------

* Numpy
* Cython (build-time only)
* Freetype2 (optional)
* Harfbuzz (optional)

Contributing
------------

We encourage contributions to celiagg!  If you would like to contribute, just
fork the repository on GitHub, make your changes, and issue a pull request.

History
-------

celiagg started as pyagg (https://github.com/erikhvatum/pyagg) but was renamed
to avoid clashing with PyAgg (https://github.com/karimbahgat/PyAgg).

celiagg is a play on Celiac (Anti grain...) and AGG (Anti-Grain Geometry).
