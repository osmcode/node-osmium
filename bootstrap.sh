#!/bin/bash

set -eu
set -o pipefail

function dep() {
    ./mason/mason install $1 $2
    ./mason/mason link $1 $2
}

# default to clang
CXX=${CXX:-clang++}

function all_deps() {
    dep boost 1.63.0
    dep expat 2.2.0
    dep bzip2 1.0.6
    dep zlib system
    dep sparsehash 2.0.2
}

MASON_VERSION="0.14.1"

function setup_mason() {
    mkdir -p ./mason
    curl -sSfL https://github.com/mapbox/mason/archive/v${MASON_VERSION}.tar.gz | tar --gunzip --extract --strip-components=1 --exclude="*md" --exclude="test*" --directory=./mason
    export PATH=$(pwd)/mason:$PATH
    export MASON_HOME=$(pwd)/mason_packages/.link
    export CXX=${CXX:-clang++}
    export CC=${CC:-clang}
}

function main() {
    setup_mason
    all_deps
    export C_INCLUDE_PATH="${MASON_HOME}/include"
    export CPLUS_INCLUDE_PATH="${MASON_HOME}/include"
    export CXXFLAGS="-I${MASON_HOME}/include"
    export LIBRARY_PATH="${MASON_HOME}/lib"
    export LDFLAGS="-L${MASON_HOME}/lib"
    echo "success: now run 'npm install --build-from-source'"
}

main

set +eu
set +o pipefail
