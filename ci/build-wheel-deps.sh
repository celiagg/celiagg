#!/bin/bash
set -e -x

LIBPNG=libpng-1.6.37
curl -L "https://downloads.sourceforge.net/project/libpng/libpng16/1.6.37/libpng-1.6.37.tar.gz" -o $LIBPNG.tar.gz
tar zxf $LIBPNG.tar.gz
pushd $LIBPNG
./configure
make -j $(nproc)
make install
popd

LIBFREETYPE=freetype-2.12.1
curl -L "https://download.savannah.gnu.org/releases/freetype/freetype-2.12.1.tar.gz" -o $LIBFREETYPE.tar.gz
tar zxf $LIBFREETYPE.tar.gz
pushd $LIBFREETYPE
./configure
make -j $(nproc)
make install
popd

LIBHARFBUZZ=harfbuzz-2.6.8
curl -L "https://github.com/harfbuzz/harfbuzz/archive/refs/tags/2.6.8.tar.gz" -o $LIBHARFBUZZ.tar.gz
tar zxf $LIBHARFBUZZ.tar.gz
pushd $LIBHARFBUZZ

# missing macro in the locally installed automake
# ref: https://github.com/pypa/manylinux/issues/731
cp /usr/share/aclocal/pkg.m4 /usr/local/share/aclocal

./autogen.sh
./configure
make -j $(nproc)
make install
popd
