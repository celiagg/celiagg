#!/bin/bash
set -e -x

PACKAGES=(
    zlib-1.2.8
    bzip2-1.0.6
    libpng-1.6.26
    freetype-2.6.5
)

URLS=(
    http://zlib.net/zlib-1.2.8.tar.gz
    http://www.bzip.org/1.0.6/bzip2-1.0.6.tar.gz
    http://download.sourceforge.net/libpng/libpng-1.6.26.tar.gz
    http://download.savannah.gnu.org/releases/freetype/freetype-2.6.5.tar.gz
)

for idx in {0..3}; do
    PACK=${PACKAGES[$idx]}
    URL=${URLS[$idx]}
    curl -L "${URL}" -o ${PACK}.tar.gz
    tar zxf ${PACK}.tar.gz
    pushd ${PACK}
    # bzip doesn't have a configure script, so ignore errors
    ./configure || true
    make
    make install
    popd
done

# Compile wheels
for PYBIN in /opt/python/cp{27,35}*/bin; do
    ${PYBIN}/pip install -r /io/travis/build-requirements.txt
    ${PYBIN}/pip wheel /io/ -w wheelhouse/
done

# Bundle external shared libraries into the wheels
for whl in wheelhouse/*.whl; do
    auditwheel repair $whl -w /io/wheelhouse/
done

# Install packages and test
for PYBIN in /opt/python/cp{27,35}*/bin/; do
    ${PYBIN}/pip install celiagg --no-index -f /io/wheelhouse
    (cd $HOME; ${PYBIN}/nosetests celiagg)
done
