#!/bin/bash

COVERAGE=${COVERAGE:-false}

if [[ ${COVERAGE} == true ]]; then
    ./mason_packages/.link/bin/cpp-coveralls \
      --gcov /usr/bin/llvm-cov-3.5 \
      --exclude __nvm \
      --exclude src/node_osmium.hpp \
      --exclude src/apply.hpp \
      --exclude src/include_v8.hpp \
      --exclude node_modules \
      --exclude tests \
      --build-root build \
      --gcov-options '\-lp' \
      --exclude doc \
      --exclude build/Release/obj/gen
fi
