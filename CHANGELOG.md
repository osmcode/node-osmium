
# Change Log

All notable changes to this project will be documented in this file.
This project adheres to [Semantic Versioning](http://semver.org/).

## [unreleased] -

### Added

### Changed

### Fixed


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

[unreleased]: https://github.com/osmcode/libosmium/compare/v0.4.4...HEAD
[0.4.4]: https://github.com/osmcode/libosmium/compare/v0.4.3...v0.4.4
[0.4.3]: https://github.com/osmcode/libosmium/compare/v0.4.2...v0.4.3
[0.4.2]: https://github.com/osmcode/libosmium/compare/v0.4.0...v0.4.2
[0.4.0]: https://github.com/osmcode/libosmium/compare/v0.1.2...v0.4.0
