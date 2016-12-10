#!/bin/bash
set -e -x

# Install FreeType
FREETYPE="freetype-2.6.5"
URL="http://download.savannah.gnu.org/releases/freetype/${FREETYPE}.tar.bz2"
curl -L "${URL}" -o ${FREETYPE}.tar.bz2
tar jxf ${FREETYPE}.tar.bz2
pushd ${FREETYPE}
./configure
make
make install
popd

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
