celiagg
=======

celiagg provides a simple, stateless canvas object that uses
[Anti-Grain Geometry 2.4](svn://svn.code.sf.net/p/agg/svn) with Cython to
render directly into a Numpy array.

A respectably modern compiler is required to build celiagg
(one supporting certain C++11 features).

Installation
------------

``python setup.py install`` should do the trick.

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
