#!/bin/bash
set -e -x

PACKAGES=(
    libpng-1.6.37
    freetype-2.12.1
    harfbuzz-3.2.0
)

URLS=(
    https://downloads.sourceforge.net/project/libpng/libpng16/1.6.37/libpng-1.6.37.tar.gz
    https://download.savannah.gnu.org/releases/freetype/freetype-2.12.1.tar.gz
    https://github.com/harfbuzz/harfbuzz/archive/refs/tags/3.2.0.tar.gz
)

# XXX: Not sure if this will work
pip install --no-cache-dir meson==0.56.0

for idx in {0..2}; do
    PACK=${PACKAGES[$idx]}
    URL=${URLS[$idx]}
    curl -L "${URL}" -o ${PACK}.tar.gz
    tar zxf ${PACK}.tar.gz
    pushd ${PACK}
    if [ "$idx" -eq "2" ]; then
        # Harfbuzz is special
        meson build
    else
        ./configure
        make
        make install
    fi
    popd
done
