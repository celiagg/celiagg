# Add conda to the $PATH
export PATH="$HOME/miniconda/bin:$PATH"
hash -r

# Activate our test env
source activate test_env

# Run the test suite
nosetests -v celiagg
