#!/usr/bin/env python3

# The MIT License (MIT)
#
# Copyright (c) 2014-2015 WUSTL ZPLAB
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
import os
import os.path as op
import subprocess
import sys

import numpy
try:
    from Cython.Distutils import build_ext
except ImportError:
    build_ext = None
    print("Cython does not appear to be installed.  Will attempt to use "
          "pre-made cpp file...")


def get_freetype_info():
    """ Use pkg-config to locate the freetype2 library installed on the system.

    If it's not installed at the system level, attempt to find it in the
    pkgconfig directory for the currently running Python interpreter.
    """
    def run_cmd(cmd, env=None):
        output = subprocess.check_output(cmd, universal_newlines=True, env=env,
                                         stderr=subprocess.DEVNULL)
        return output.split()

    def collect_data(env=None):
        cmd_prefix = ['pkg-config', 'freetype2']
        commands = {'cflags': ['--cflags'], 'ldflags': ['--libs']}

        data = {}
        try:
            for key, args in commands.items():
                data[key] = run_cmd(cmd_prefix + args, env=env)
        except subprocess.CalledProcessError:
            pass
        return data

    data = collect_data()
    if len(data) < 2:
        # Try again with the Python env's pkgconfig directory added
        env = os.environ.copy()
        env['PKG_CONFIG_PATH'] = op.join(sys.exec_prefix, 'lib', 'pkgconfig')
        data = collect_data(env=env)

    if len(data) < 2:
        msg = ("Failed to execute pkg-config freetype2.  If freetype is "
               "installed in standard system locations, it may work to run "
               "this script with --no-freetype-pkg-config.  Otherwise, "
               "appropriate CFLAGS and LDFLAGS environment variables must be "
               "set.")
        sys.stderr.write(msg)
        exit(-1)
    return data['cflags'], data['ldflags']


cpp_sources = [
    'cython/ndarray_canvas.cpp',
    'agg-svn/agg-2.4/agg2d/agg2d.cpp',
    'agg-svn/agg-2.4/src/agg_arc.cpp',
    'agg-svn/agg-2.4/src/agg_arrowhead.cpp',
    'agg-svn/agg-2.4/src/agg_bezier_arc.cpp',
    'agg-svn/agg-2.4/src/agg_bspline.cpp',
    'agg-svn/agg-2.4/src/agg_color_rgba.cpp',
    'agg-svn/agg-2.4/src/agg_curves.cpp',
    'agg-svn/agg-2.4/src/agg_embedded_raster_fonts.cpp',
    'agg-svn/agg-2.4/src/agg_gsv_text.cpp',
    'agg-svn/agg-2.4/src/agg_image_filters.cpp',
    'agg-svn/agg-2.4/src/agg_line_aa_basics.cpp',
    'agg-svn/agg-2.4/src/agg_line_profile_aa.cpp',
    'agg-svn/agg-2.4/src/agg_rounded_rect.cpp',
    'agg-svn/agg-2.4/src/agg_sqrt_tables.cpp',
    'agg-svn/agg-2.4/src/agg_trans_affine.cpp',
    'agg-svn/agg-2.4/src/agg_trans_double_path.cpp',
    'agg-svn/agg-2.4/src/agg_trans_single_path.cpp',
    'agg-svn/agg-2.4/src/agg_trans_warp_magnifier.cpp',
    'agg-svn/agg-2.4/src/agg_vcgen_bspline.cpp',
    'agg-svn/agg-2.4/src/agg_vcgen_contour.cpp',
    'agg-svn/agg-2.4/src/agg_vcgen_dash.cpp',
    'agg-svn/agg-2.4/src/agg_vcgen_markers_term.cpp',
    'agg-svn/agg-2.4/src/agg_vcgen_smooth_poly1.cpp',
    'agg-svn/agg-2.4/src/agg_vcgen_stroke.cpp',
    'agg-svn/agg-2.4/src/agg_vpgen_clip_polygon.cpp',
    'agg-svn/agg-2.4/src/agg_vpgen_clip_polyline.cpp',
    'agg-svn/agg-2.4/src/agg_vpgen_segmentator.cpp',
]

cython_source = 'cython/pyagg.pyx'
cython_source_deps = [
    'cython/_agg2d.pxd',
    'cython/_pyagg.pxd',
    'cython/agg2d.pxi',
    'cython/ndarray_canvas.pxi']
cythoned_source = 'cython/pyagg.cpp'

include_dirs = ['agg-svn/agg-2.4/include',
                'agg-svn/agg-2.4',
                numpy.get_include()]
extra_compile_args = []
extra_link_args = []
define_macros = []

if '--with-freetype' in sys.argv:
    idx = sys.argv.index('--with-freetype')
    del sys.argv[idx]
    with_freetype = True
else:
    if os.name == 'nt':
        with_freetype = False
    else:
        with_freetype = True

if with_freetype:
    if '--no-freetype-pkg-config' in sys.argv:
        idx = sys.argv.index('--no-freetype-pkg-config')
        del sys.argv[idx]
    else:
        cflags, ldflags = get_freetype_info()
        extra_compile_args.extend(cflags)
        extra_link_args.extend(ldflags)
    include_dirs.append('agg-svn/agg-2.4/font_freetype')
    cpp_sources.append('agg-svn/agg-2.4/font_freetype/agg_font_freetype.cpp')
    define_macros.append(('AGG2D_USE_FREETYPE', 1))
else:
    include_dirs.append('agg-svn/agg-2.4/font_win32_tt')
    cpp_sources.append('agg-svn/agg-2.4/font_win32_tt/agg_font_win32_tt.cpp')

if build_ext is not None:
    # Run Cython
    extension = Extension('pyagg',
                          sources=[cython_source] + cpp_sources,
                          include_dirs=include_dirs,
                          define_macros=define_macros,
                          language='c++',
                          depends=cython_source_deps,
                          extra_compile_args=extra_compile_args,
                          extra_link_args=extra_link_args)
    kwargs = {'ext_modules': [extension], 'cmdclass': {'build_ext': build_ext}}
else:
    # Don't run Cython
    extension = Extension('pyagg',
                          sources=[cythoned_source] + cpp_sources,
                          include_dirs=include_dirs,
                          define_macros=define_macros,
                          language='c++',
                          extra_compile_args=extra_compile_args,
                          extra_link_args=extra_link_args)
    kwargs = {'ext_modules': [extension]}

setup(name='pyagg', **kwargs)
