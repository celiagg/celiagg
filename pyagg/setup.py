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
#          John Wiggins <john.wiggins@xfel.eu>

from __future__ import print_function
import os
import os.path as op
import platform
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

# These are added to the environment by the main setup.py script
with_text_rendering = os.environ.get('PYAGG_TEXT_RENDERING', '0') == '1'
with_pkgconfig = os.environ.get('PYAGG_USE_PKGCONFIG', '0') == '1'


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
               "set.\n\n"
               "If you wish to disable text rendering, you can re-run this "
               "script with the --no-text-rendering flag.")
        print(msg, file=sys.stderr)
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
    pyagg_cython_source = op.join(base_path, '_pyagg.pyx')
    pyagg_sources = ['font.cpp', 'glyph_iter.cpp', 'image.cpp', 'paint.cpp',
                     'vertex_source.cpp', '_pyagg.cpp']

    include_dirs = ['agg-svn/agg-2.4/include',
                    'agg-svn/agg-2.4',
                    numpy.get_include()]
    extra_compile_args = ['-Wfatal-errors', '-Wno-unused-function']
    extra_link_args = []
    define_macros = []

    if with_text_rendering:
        if platform.system() == 'Windows':
            include_dirs.append('agg-svn/agg-2.4/font_win32_tt')
            font_source = 'agg-svn/agg-2.4/font_win32_tt/agg_font_win32_tt.cpp'
        else:
            if with_pkgconfig:
                cflags, ldflags = get_freetype_info()
                extra_compile_args.extend(cflags)
                extra_link_args.extend(ldflags)
            define_macros.append(('_USE_FREETYPE', 1))
            include_dirs.append('agg-svn/agg-2.4/font_freetype')
            font_source = 'agg-svn/agg-2.4/font_freetype/agg_font_freetype.cpp'
        sources.append(font_source)
        define_macros.append(('_ENABLE_TEXT_RENDERING', 1))

    cython_compile_env = {'_ENABLE_TEXT_RENDERING': with_text_rendering}

    ext_kwargs = {
        'include_dirs': include_dirs,
        'define_macros': define_macros,
        'language':  'c++',
        'extra_compile_args': extra_compile_args,
        'extra_link_args': extra_link_args
    }

    # Installing from an SDIST is special...
    cpp_pyagg = op.join('pyagg', '_pyagg.cpp')
    pyx_pyagg = op.join('pyagg', '_pyagg.pyx')
    is_sdist = op.exists(cpp_pyagg) and not op.exists(pyx_pyagg)

    # Run Cython first if this is a development version
    if not is_sdist and cythonize is not None:
        cythonize(pyagg_cython_source, language='c++',
                  compile_time_env=cython_compile_env)

    config = Configuration('pyagg', parent_package, top_path)
    config.add_extension('_pyagg', sources=pyagg_sources + sources,
                         **ext_kwargs)
    config.add_subpackage('tests')
    return config

if __name__ == '__main__':
    setup(**(configuration(top_path='').todict()))
