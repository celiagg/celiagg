#!/bin/bash
set -e -x

PACKAGES=(
    zlib-1.2.8
    libpng-1.6.26
    freetype-2.6.5
)

URLS=(
    https://downloads.sourceforge.net/project/libpng/zlib/1.2.8/zlib-1.2.8.tar.gz
    http://download.sourceforge.net/libpng/libpng-1.6.26.tar.gz
    http://download.savannah.gnu.org/releases/freetype/freetype-2.6.5.tar.gz
)

for idx in {0..2}; do
    PACK=${PACKAGES[$idx]}
    URL=${URLS[$idx]}
    curl -L "${URL}" -o ${PACK}.tar.gz
    tar zxf ${PACK}.tar.gz
    pushd ${PACK}
    ./configure
    make
    make install
    popd
done
