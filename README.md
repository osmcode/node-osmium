# node-osmium

Flexible Javascript library for working with OpenStreetMap data.

Provides bindings to the [libosmium](https://github.com/osmcode/libosmium)
C++ library.

[![NPM](https://nodei.co/npm/osmium.png?downloads=true&downloadRank=true)](https://nodei.co/npm/osmium/)
[![Build Status](https://secure.travis-ci.org/osmcode/node-osmium.png)](http://travis-ci.org/osmcode/node-osmium)
[![Build status](https://ci.appveyor.com/api/projects/status/g0j361782j0h0cge?svg=true)](https://ci.appveyor.com/project/Mapbox/node-osmium)
[![Coverage Status](https://coveralls.io/repos/osmcode/node-osmium/badge.svg?branch=coverage)](https://coveralls.io/r/osmcode/node-osmium?branch=coverage)
[![Dependencies](https://david-dm.org/osmcode/node-osmium.png)](https://david-dm.org/osmcode/node-osmium)

## Should you use node-osmium?

If you want top performance use libosmium directly in C++. These node-osmium bindings, due to the expense of passing objects from C++ to Javascript, are much slower than working in C++ directly. Consider `node-osmium` only for small extracts and prototyping. For large extracts or planet processing we recommend leveraging the [libosmium C++ API](http://docs.osmcode.org/libosmium-manual/) instead of using node-osmium.

## Is node-osmium actively developed?

@springmeyer and @joto are maintaining node-osmium but not actively adding features. We will consider pull requests adding features only when they come with very solid tests, add very clear value to the bindings, and seem easy to maintain.


## Depends

 - Node.js v0.10.x, v4.x, or v5.x (v5 and v5 supported only with node-osmium >= 0.5.x)
 - libosmium (https://github.com/osmcode/libosmium)
 - protozero (https://github.com/mapbox/protozero)
 - Mocha (http://visionmedia.github.io/mocha/, for tests)

## Installing

By default, binaries are provided and no external dependencies or compile is
needed.

Just do:

```shell
npm install osmium
```

We currently provide binaries for 64 bit OS X and 64 bit Linux. Running `npm
install` on other platforms will fall back to a source compile (see
`Developing` below for build details).


## Usage

See [the tutorial](doc/tutorial.md) for an introduction. There are some demo
applications in the 'demo' directory. See the [README.md](demo/README.md)
there. You can also have a look at the tests in the `test` directory.


## Developing

If you wish to develop on `node-osmium` you can check out the code and then
build like:

```shell
git clone https://github.com/osmcode/node-osmium.git
cd node-osmium
make
```

Use `make debug` to build with debug information. Use `make coverage` to build
with code coverage.

Use `make VERBOSE=1` to output compiler calls used etc.


## Testing

    npm install mocha
    make test

### Source build dependencies

 - Compiler that supports `-std=c++11` (>= clang++ 3.4 || >= g++ 4.8)
 - [libosmium](https://github.com/osmcode/libosmium)
 - [Boost](http://www.boost.org/) >= 1.46 with development headers
 - zlib

See also the dependency information for the Osmium library.

Set dependencies up on Ubuntu Precise (12.04) like:

```shell
sudo apt-add-repository --yes ppa:chris-lea/node.js
sudo apt-add-repository --yes ppa:ubuntu-toolchain-r/test
sudo apt-get -y update
sudo apt-get -y install git gcc-4.8 g++-4.8 build-essential nodejs
sudo apt-get -y install libboost-dev zlib1g-dev libexpat1-dev libsparsehash-dev
export CC=gcc-4.8
export CXX=g++-4.8
git clone https://github.com/scrosby/OSM-binary.git
cd OSM-binary/src
make && sudo make install
```

Set dependencies up on OS X like:

```shell
git clone https://github.com/mapnik/mapnik-packaging.git
cd mapnik-packaging/osx
export CXX11=true
source MacOSX.sh
./scripts/build_bzip2.sh
./scripts/build_expat.sh
./scripts/build_google_sparsetable.sh
./scripts/build_boost.sh --with-test --with-program_options
./scripts/build_protobuf.sh
./scripts/build_osm-pbf.sh
# NOTE: in the same terminal then run the build commands
# Or from a different terminal re-run `source MacOSX.sh`
```

## License

node-osmium is available under the Boost Software License. See LICENSE.txt for
details.

## Contact

Please open bug reports on https://github.com/osmcode/node-osmium/issues. You
can ask questions on the
[OSM developer mailing list](https://lists.openstreetmap.org/listinfo/dev)
or on [OFTC net IRC channel #osm-dev](https://wiki.openstreetmap.org/wiki/Irc).

## Authors

 - Dane Springmeyer (dane@mapbox.com)
 - Jochen Topf (jochen@topf.org)

