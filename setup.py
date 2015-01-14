#!/usr/bin/env python3

# The MIT License (MIT)
#
# Copyright (c) 2014 WUSTL ZPLAB
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
# Authors: Erik Hvatum <ice.rikh@gmail.com>

from distutils.core import setup
from distutils.extension import Extension
import numpy
import sys

cpp_sources = [
    'cython/ndarray_canvas.cpp',
    'agg-svn/agg-2.4/src/agg_bezier_arc.cpp',
    'agg-svn/agg-2.4/src/agg_bspline.cpp',
    'agg-svn/agg-2.4/src/agg_curves.cpp',
    'agg-svn/agg-2.4/src/agg_trans_affine.cpp',
    'agg-svn/agg-2.4/src/agg_vcgen_bspline.cpp',
    'agg-svn/agg-2.4/src/agg_vcgen_contour.cpp',
    'agg-svn/agg-2.4/src/agg_vcgen_stroke.cpp']

cython_source = 'cython/pyagg.pyx'
cythoned_source = 'cython/pyagg.cpp'

include_dirs = ['agg-svn/agg-2.4/include',
                numpy.get_include()]

try:
    from Cython.Build import cythonize
    from Cython.Compiler import DebugFlags
    from Cython.Distutils import build_ext

    ext_modules = [Extension('pyagg',
                             sources = [cython_source,] + cpp_sources,
                             include_dirs = include_dirs,
                             language = 'c++',
                             )]

    setup(name = 'pyagg',
          cmdclass = {'build_ext' : build_ext},
          ext_modules = cythonize(ext_modules, include_path=['cython'], language='c++'))
except ImportError:
    print('Cython does not appear to be installed.  Attempting to use pre-made cpp file...')

    ext_modules = [Extension('pyagg',
                             sources = [cythoned_source,] + cpp_sources,
                             include_dirs = include_dirs,
                             language = 'c++',
                             )]

    setup(name = 'pyagg',
          ext_modules = ext_modules)
