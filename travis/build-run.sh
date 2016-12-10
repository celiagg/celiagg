#!/bin/bash

# Run the build-wheels script in the docker container
docker run --rm -v `pwd`:/io $DOCKER_IMAGE /io/travis/build-wheels.sh

# Install conda
URL="https://repo.continuum.io/miniconda/Miniconda3-latest-Linux-x86_64.sh"
wget "${URL}" -O miniconda.sh
bash miniconda.sh -b -p $HOME/miniconda

export PATH="$HOME/miniconda/bin:$PATH"
hash -r

conda config --set always_yes yes --set changeps1 no
conda update -q conda
conda create -q -n twine_env python=3.5 twine
source activate twine_env

# Upload to PyPI
twine upload -u jwiggins wheelhouse/celiagg-*.whl
