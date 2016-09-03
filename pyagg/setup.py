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
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
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

from __future__ import print_function
import os
import os.path as op
import subprocess
import sys

import numpy
from numpy.distutils.core import setup
from numpy.distutils.misc_util import Configuration

try:
    from Cython.Build import cythonize
except ImportError:
    cythonize = None
    print("Cython does not appear to be installed.  Will attempt to use "
          "pre-made cpp file...")


def get_freetype_info():
    """ Use pkg-config to locate the freetype2 library installed on the system.

    If it's not installed at the system level, attempt to find it in the
    pkgconfig directory for the currently running Python interpreter.
    """
    def run_cmd(cmd, env=None):
        with open("/dev/null", "w") as dev_null:
            output = subprocess.check_output(cmd, universal_newlines=True,
                                             env=env, stderr=dev_null.fileno())
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


def configuration(parent_package='', top_path=None):
    sources = [
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
    base_path = op.abspath(op.dirname(__file__))
    agg2d_cython_source = op.join(base_path, 'agg2d.pyx')
    pyagg_cython_source = op.join(base_path, '_pyagg.pyx')
    agg2d_sources = ['agg-svn/agg-2.4/agg2d/agg2d.cpp', 'agg2d.cpp']
    pyagg_sources = ['glyph_iter.cpp', 'paint.cpp', 'text.cpp', '_pyagg.cpp']

    include_dirs = ['agg-svn/agg-2.4/include',
                    'agg-svn/agg-2.4',
                    numpy.get_include()]
    extra_compile_args = ['-Wfatal-errors', '-Wno-unused-function']
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
        sources.append('agg-svn/agg-2.4/font_freetype/agg_font_freetype.cpp')
        define_macros.extend([('AGG2D_USE_FREETYPE', 1), ('_USE_FREETYPE', 1)])
    else:
        include_dirs.append('agg-svn/agg-2.4/font_win32_tt')
        sources.append('agg-svn/agg-2.4/font_win32_tt/agg_font_win32_tt.cpp')

    ext_kwargs = {
        'include_dirs': include_dirs,
        'define_macros': define_macros,
        'language':  'c++',
        'extra_compile_args': extra_compile_args,
        'extra_link_args': extra_link_args
    }

    if cythonize is not None:
        # Run Cython first
        cythonize(pyagg_cython_source, language='c++')
        cythonize(agg2d_cython_source, language='c++')

    config = Configuration('pyagg', parent_package, top_path)
    config.add_extension('_pyagg', sources=pyagg_sources + sources,
                         **ext_kwargs)
    config.add_extension('agg2d', sources=agg2d_sources + sources,
                         **ext_kwargs)
    config.add_subpackage('tests')
    return config

if __name__ == '__main__':
    setup(**(configuration(top_path='').todict()))
