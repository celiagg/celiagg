#!/bin/bash
set -e -x

PACKAGES=(
    zlib-1.2.11
    libpng-1.6.37
    freetype-2.10.4
    harfbuzz-2.6.4
)

URLS=(
    https://www.zlib.net/zlib-1.2.11.tar.gz
    https://downloads.sourceforge.net/project/libpng/libpng16/1.6.37/libpng-1.6.37.tar.gz
    https://download.savannah.gnu.org/releases/freetype/freetype-2.10.4.tar.gz
    https://github.com/harfbuzz/harfbuzz/archive/2.6.4.tar.gz
)

for idx in {0..3}; do
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
