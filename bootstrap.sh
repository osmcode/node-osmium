#!/bin/bash

function dep() {
    ./.mason/mason install $1 $2
    ./.mason/mason link $1 $2
}

# default to clang
CXX=${CXX:-clang++}

function all_deps() {
    dep boost 1.57.0 &
    dep expat 2.1.0 &
    dep osmpbf 1.3.3 &
    dep protobuf 2.6.1 &
    dep bzip 1.0.6 &
    dep zlib system &
    dep sparsehash 2.0.2 & 
    wait
}

function main() {
    if [[ ! -d ./.mason ]]; then
        git clone --depth 1 https://github.com/mapbox/mason.git ./.mason
    fi
    export MASON_DIR=$(pwd)/.mason
    export MASON_HOME=$(pwd)/mason_packages/.link
    if [[ ! -d ${MASON_HOME} ]]; then
        all_deps
    fi
    export C_INCLUDE_PATH="${MASON_HOME}/include"
    export CPLUS_INCLUDE_PATH="${MASON_HOME}/include"
    export LIBRARY_PATH="${MASON_HOME}/lib"
    echo "success: now run 'npm install --build-from-source'"
}

main