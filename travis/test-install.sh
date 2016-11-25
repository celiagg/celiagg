# Create an environment with our build/test requirements
conda create -q -n test_env python=${PYTHON} numpy cython freetype nose

# Activate it
source activate test_env

# Install celiagg inplace
pip install -e . ${BUILD_ARGS}
