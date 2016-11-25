# Install miniconda
URL="https://repo.continuum.io/miniconda/Miniconda3-latest-${MINICONDA_OS}-x86_64.sh"
wget "${URL}" -O miniconda.sh
bash miniconda.sh -b -p $HOME/miniconda

# Add it to the $PATH
export PATH="$HOME/miniconda/bin:$PATH"
hash -r

# Make it whole
conda config --set always_yes yes --set changeps1 no
conda update -q conda
conda info -a
