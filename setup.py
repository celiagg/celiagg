# The MIT License (MIT)
#
# Copyright (c) 2014-2016 WUSTL ZPLAB
# Copyright (c) 2016-2021 Celiagg Contributors
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
#          John Wiggins <jwiggins@enthought.com>
from distutils.command.sdist import sdist as _sdist
import os
import os.path as op
import platform
import subprocess
import sys

import numpy
from setuptools import Extension, setup

try:
    from Cython.Distutils import build_ext as cython_build_ext
except ImportError:
    cython_build_ext = None
    print("Cython does not appear to be installed.  Will attempt to use "
          "pre-made cpp file...")

# Disable text rendering with this option
if '--no-text-rendering' in sys.argv:
    del sys.argv[sys.argv.index('--no-text-rendering')]
    with_text_rendering = False
else:
    with_text_rendering = True
    with_pkgconfig = True
    # Disable pkg-config use with this option
    if '--no-freetype-pkg-config' in sys.argv:
        del sys.argv[sys.argv.index('--no-freetype-pkg-config')]
        with_pkgconfig = False


class PatchedSdist(_sdist):
    """ Make sure the compiled Cython files are included
    """
    def run(self):
        from Cython.Build import cythonize

        cythonize([os.path.join('celiagg', '_celiagg.pyx')])
        _sdist.run(self)


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
        except (subprocess.CalledProcessError, OSError):
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

        # NOTE: Avoid exiting when pip is running an egg_info command. Without
        # this, it's not possible to avoid freetype when installing from pip.
        if 'egg_info' not in sys.argv:
            exit(-1)
        else:
            return [], []
    return data['cflags'], data['ldflags']


def create_extension():
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
    celiagg_sources = ['canvas_impl.cpp', 'font_cache.cpp', 'font.cpp',
                       'glyph_iter.cpp', 'image.cpp', 'paint.cpp',
                       'vertex_source.cpp']
    celiagg_sources = [os.path.join('celiagg', p) for p in celiagg_sources]

    # SDIST should build without Cython
    if cython_build_ext is not None:
        celiagg_sources.append(os.path.join('celiagg', '_celiagg.pyx'))
    else:
        celiagg_sources.append(os.path.join('celiagg', '_celiagg.cpp'))

    include_dirs = ['agg-svn/agg-2.4/include',
                    'agg-svn/agg-2.4',
                    'celiagg',
                    numpy.get_include()]
    if platform.system() == "Windows":
        # Visual studio does not support/need these
        extra_compile_args = []
    else:
        extra_compile_args = [
           '-Wfatal-errors',
           '-Wno-unused-function',
           '-std=c++11',
        ]
    extra_link_args = []
    define_macros = [
        ('NPY_NO_DEPRECATED_API', 'NPY_1_7_API_VERSION'),
    ]

    if with_text_rendering:
        if platform.system() == 'Windows':
            extra_link_args.extend(['Gdi32.lib', 'User32.lib'])
            include_dirs.append('agg-svn/agg-2.4/font_win32_tt')
            font_source = 'agg-svn/agg-2.4/font_win32_tt/agg_font_win32_tt.cpp'
        else:
            if with_pkgconfig:
                cflags, ldflags = get_freetype_info()
                extra_compile_args.extend(cflags)
                extra_link_args.extend(ldflags)
            define_macros.append(('_USE_FREETYPE', None))
            include_dirs.append('agg-svn/agg-2.4/font_freetype')
            font_source = 'agg-svn/agg-2.4/font_freetype/agg_font_freetype.cpp'
        sources.append(font_source)
        define_macros.append(('_ENABLE_TEXT_RENDERING', None))

    return Extension(
        'celiagg._celiagg',
        sources=celiagg_sources + sources,
        include_dirs=include_dirs,
        define_macros=define_macros,
        extra_compile_args=extra_compile_args,
        extra_link_args=extra_link_args,
        language='c++',
    )


with open('README.rst', 'r') as fp:
    long_description = fp.read()

requires = ['numpy']
if sys.platform not in ('win32', 'cygwin'):
    # Windows doesn't use FreeType.
    requires.append('freetype')

cmdclass = {'sdist': PatchedSdist}
if cython_build_ext is not None:
    cmdclass['build_ext'] = cython_build_ext

setup(
    name='celiagg',
    license='MIT',
    version='2.1.3',
    description='Anti-Grain Geometry for Python 3 with Cython',
    long_description=long_description,
    long_description_content_type='text/x-rst',
    url='https://github.com/celiagg/celiagg',
    classifiers=[
        'Development Status :: 5 - Production/Stable',
        'Environment :: Console',
        'Intended Audience :: Developers',
        'License :: OSI Approved :: MIT License',
        'Programming Language :: C++',
        'Programming Language :: Cython',
        'Programming Language :: Python',
        'Programming Language :: Python :: 3',
        'Topic :: Software Development :: Libraries',
        'Operating System :: Microsoft :: Windows',
        'Operating System :: POSIX',
        'Operating System :: Unix',
        'Operating System :: MacOS',
    ],
    requires=requires,
    cmdclass=cmdclass,
    ext_modules=[create_extension()],
    packages=['celiagg', 'celiagg.tests'],
    package_data={
        'celiagg': ['data/*'],
    },
)
