#!/bin/bash

function dep() {
    ./.mason/mason install $1 $2
    ./.mason/mason link $1 $2
}

# default to clang
CXX=${CXX:-clang++}

function all_deps() {
    dep boost 1.61.0 &
    dep expat 2.1.1 &
    dep bzip 1.0.6 &
    dep zlib system &
    dep sparsehash 2.0.2 &
    wait
}

MASON_VERSION="b709931"

function setup_mason() {
    if [[ ! -d ./.mason ]]; then
        git clone https://github.com/mapbox/mason.git ./.mason
        (cd ./.mason && git checkout ${MASON_VERSION})
    else
        echo "Updating to latest mason"
        (cd ./.mason && git fetch && git checkout ${MASON_VERSION})
    fi
    export MASON_HOME=$(pwd)/mason_packages/.link
    export PATH=$(pwd)/.mason:$PATH
    export CXX=${CXX:-clang++}
    export CC=${CC:-clang}
}

function main() {
    setup_mason
    if [[ ! -d ${MASON_HOME} ]]; then
        all_deps
    fi
    export C_INCLUDE_PATH="${MASON_HOME}/include"
    export CPLUS_INCLUDE_PATH="${MASON_HOME}/include"
    export CXXFLAGS="-I${MASON_HOME}/include"
    export LIBRARY_PATH="${MASON_HOME}/lib"
    export LDFLAGS="-L${MASON_HOME}/lib"
    echo "success: now run 'npm install --build-from-source'"
}

main
