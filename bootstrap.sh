#!/bin/bash

bash ./version.sh
python3 ./make_Makemodule.py

if [ ! $# -eq 0  ]; then
    echo "Applying specified tag to version string: $1"

    version=$(cat build-aux/casm_version.txt)
    reversion=$1-${version#*-}
    echo $reversion > build-aux/casm_version.txt
fi

autoreconf -ivf
