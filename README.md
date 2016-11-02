pyagg
=====

pyagg provides a simple, stateless canvas object that uses [Anti-Grain Geometry 2.4] (svn://svn.code.sf.net/p/agg/svn)
with Cython to render directly into a Numpy array.

A respectably modern compiler is required to build pyagg (one supporting certain C++11
features).

# Installation

## setup.py
`python setup.py install` should do the trick.

## Dependencies
* Numpy
* Cython
* Freetype2 (optional)
