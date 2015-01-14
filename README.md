pyagg
=====

pyagg provides a simple, stateless canvas object that uses [Anti-Grain Geometry 2.4] (svn://svn.code.sf.net/p/agg/svn)
with Cython to render directly into a Numpy array.

A respectably modern compiler is required to build pyagg (one supporting certain C++11
features).

# Installation

## Easy: setup.py
`python3 setup.py install` should do the trick.

## Alternative: CMake
The included CMakeLists.txt files are intended for development use, in order to
ease generation of XCode and Visual Studio project files for pyagg, and are for
building only - no provision is made for placing the built Python module binary
in site-packages.  This may be done manually, of course.  Instead, the binary
is copied to pyagg's source directory, as would be done by
python3 setup.py build_ext --inplace.
