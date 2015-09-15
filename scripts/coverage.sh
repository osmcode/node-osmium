#!/bin/bash

COVERAGE=${COVERAGE:-false}

if [[ ${COVERAGE} == true ]]; then
    ./mason_packages/.link/bin/cpp-coveralls \
      --exclude src/node_osmium.hpp \
      --exclude src/apply.hpp \
      --exclude src/include_v8.hpp \
      --exclude node_modules \
      --exclude tests \
      --build-root build \
      --gcov-options '\-lp' \
      --exclude doc \
      --exclude build/Release/obj/gen > /dev/null
fi
