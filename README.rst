celiagg
=======

.. image:: https://img.shields.io/pypi/v/celiagg.svg
  :target: https://pypi.python.org/pypi/celiagg
  :alt: PyPI

.. image:: https://travis-ci.org/celiagg/celiagg.svg?branch=master
   :target: https://travis-ci.org/celiagg/celiagg
   :alt: Build status

.. image:: https://ci.appveyor.com/api/projects/status/1d7wb65intsxrn90
   :target: https://ci.appveyor.com/project/jwiggins/celiagg
   :alt: Appveyor Build status

celiagg provides a simple, stateless canvas object that uses
Anti-Grain Geometry 2.4 <svn://svn.code.sf.net/p/agg/svn> with Cython to
render directly into a Numpy array.

A respectably modern compiler is required to build celiagg
(one supporting certain C++11 features).

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
* Cython
* Freetype2 (optional)

Contributing
------------

We encourage contributions to celiagg!  If you would like to contribute, just
fork the repository on GitHub, make your changes, and issue a pull request.

History
-------

celiagg started as pyagg (https://github.com/erikhvatum/pyagg) but was renamed
to avoid clashing with PyAgg (https://github.com/karimbahgat/PyAgg).

celiagg is a play on Celiac (Anti grain...) and AGG (Anti-Grain Geometry).
