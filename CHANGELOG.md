
# Change Log

All notable changes to this project will be documented in this file.
This project adheres to [Semantic Versioning](http://semver.org/).

## [unreleased] -

### Added

### Changed

### Fixed

## [0.5.6] - 2017-08-09

### Changed

- Upgraded to libosmium v2.12.2.
- Upgraded to nan v2.6.2
- Upgraded to node-pre-gyp v0.6.36
- Upgraded to boost 1.63.0
- Upgraded to expat 2.2.0
- Now compiling with clang++ 4.0.1
- No longer providing binaries for node v5

## [0.5.5] - 2017-08-09

### Changed

- Now building binaries against libstdc++-4.8-dev (which avoids a dependency on >= GLIBCXX 3.4.20)
    - Should work on >= ubuntu trusty  and amazon linux without needing to upgrade libstdc++
- Added node v8 binaries

## [0.5.4] - 2016-11-21

### Changed

- Using newest libosmium, version 2.10.3.


## [0.5.3] - 2016-09-19

### Changed

- Using newest libosmium, version 2.9.0.


## [0.5.2] - 2016-07-28

### Fixed

- Now properly statically linking bz2 on linux (no longer accidentally depends
  on shared libbz2)
- Fixed memory leak in handler which regressed in 0.5.0.
  (https://github.com/osmcode/node-osmium/issues/79)

### Added

- Add area support to `object_to_entity_bits()` function.
  (https://github.com/osmcode/node-osmium/pull/49)


## [0.5.1] - 2016-07-21

### Added

- Support for node v6.

### Changed

- Updated to libosmium 2.7.2


## [0.5.0] - 2016-03-31

### Added

- Support for node v4 and v5 (now ported to use Nan@2.2.1)

### Changed

- Updated to libosmium 2.6.1


## [0.4.5] - 2016-02-10

### Changed

- Updated to libosmium 2.6.0

### Fixed

- Fixed abort when invalid data is based to an `osmium.Buffer`

## [0.4.4] - 2015-11-10

### Fixed

- Removes bindings from npm bundle to fix installation via node-pre-gyp


## [0.4.3] - 2015-11-10

### Added

- GeoJSON support for Areas
- Lots of tests for Areas

### Fixed

- Issue with buffer not being flushed which resulted in some Areas
  not showing up in the handler.


## [0.4.2] - 2015-09-01

### Changed

- Updated to libosmium 2.4.1


## [0.4.1] - unreleased due to version number confusion

## [0.4.0] - 2015-08-19

### Added

- Added `osmium.Stream`
- Added `osmium.FlexReader`
- Added `osmium.Filter`

### Changed

- Renamed `osmium.Reader` to `osmium.BasicReader`
- Updated to libosmium@33d479d7eada041
- Ported build system to [mason](https://github.com/mapbox/mason)
- Upgraded node-pre-gyp to 0.6.6
- Preliminary support for running on windows


## [0.1.2] - 2014-04-29

### Changed

- Upgraded node-pre-gyp to 0.5.10

[unreleased]: https://github.com/osmcode/libosmium/compare/v0.5.4...HEAD
[0.5.4]: https://github.com/osmcode/libosmium/compare/v0.5.3...v0.5.4
[0.5.3]: https://github.com/osmcode/libosmium/compare/v0.5.2...v0.5.3
[0.5.2]: https://github.com/osmcode/libosmium/compare/v0.5.1...v0.5.2
[0.5.1]: https://github.com/osmcode/libosmium/compare/v0.5.0...v0.5.1
[0.5.0]: https://github.com/osmcode/libosmium/compare/v0.4.5...v0.5.0
[0.4.5]: https://github.com/osmcode/libosmium/compare/v0.4.4...v0.4.5
[0.4.4]: https://github.com/osmcode/libosmium/compare/v0.4.3...v0.4.4
[0.4.3]: https://github.com/osmcode/libosmium/compare/v0.4.2...v0.4.3
[0.4.2]: https://github.com/osmcode/libosmium/compare/v0.4.0...v0.4.2
[0.4.0]: https://github.com/osmcode/libosmium/compare/v0.1.2...v0.4.0

