#!/bin/bash

# Add conda to the $PATH
export PATH="$HOME/miniconda/bin:$PATH"
hash -r

# Create an environment with our build/test requirements
conda create -q -n test_env python=${PYTHON} numpy cython freetype

# Activate it
source activate test_env

# Install celiagg inplace
pip install -e . ${BUILD_ARGS}
