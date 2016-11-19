# The MIT License (MIT)
#
# Copyright (c) 2014-2016 WUSTL ZPLAB
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
import os
import sys

from numpy.distutils.core import setup
from numpy.distutils.misc_util import Configuration

# Disable text rendering with this option
if '--no-text-rendering' in sys.argv:
    del sys.argv[sys.argv.index('--no-text-rendering')]
    os.environ['CELIAGG_TEXT_RENDERING'] = '0'
else:
    os.environ['CELIAGG_TEXT_RENDERING'] = '1'
    os.environ['CELIAGG_USE_PKGCONFIG'] = '1'
    if '--no-freetype-pkg-config' in sys.argv:
        del sys.argv[sys.argv.index('--no-freetype-pkg-config')]
        os.environ['CELIAGG_USE_PKGCONFIG'] = '0'


def configuration(parent_package='', top_path=None):
    config = Configuration(None, parent_package, top_path)
    config.set_options(
        ignore_setup_xxx_py=True,
        assume_default_configuration=True,
        delegate_options_to_subpackages=True,
        quiet=True)

    config.add_subpackage('celiagg')
    return config

with open('README.rst', 'r') as fp:
    long_description = fp.read()

setup(
    name='celiagg',
    configuration=configuration,
    license='MIT',
    version='0.1.2',
    description='Anti-Grain Geometry for Python (2 & 3) with Cython',
    long_description=long_description,
    url='https://github.com/celiagg/celiagg',
    classifiers=[
        'Development Status :: 4 - Beta',
        'Environment :: Console',
        'Intended Audience :: Developers',
        'License :: OSI Approved :: MIT License',
        'Programming Language :: C++',
        'Programming Language :: Cython',
        'Programming Language :: Python',
        'Programming Language :: Python :: 2',
        'Programming Language :: Python :: 3',
        'Topic :: Software Development :: Libraries',
        'Operating System :: Microsoft :: Windows',
        'Operating System :: POSIX',
        'Operating System :: Unix',
        'Operating System :: MacOS',
    ],
    requires=['numpy', 'freetype'],
    packages=['celiagg', 'celiagg.tests']
)
