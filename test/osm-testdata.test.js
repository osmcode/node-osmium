var fs = require('fs');
var path = require('path');
var assert = require('assert');

var testData = require('osm-testdata');
var wellknown = require('wellknown');
var flat = require('geojson-coords');
var difference = require('turf-difference');

var osmium = require('..');

// Check two areas (as geojson geometries) for equality:
// - they must have the same total number of coordinates
// - a spatial difference operation should leave no remainder
function areasEqual(geom1, geom2) {
    // if (flat(geom1).length !== flat(geom2).length) return false;
    var diff = difference(geom1, geom2);
    return !diff;
}


// Parse files from osm-testdata repository
var tests = ['1', '3', '7', '9'].reduce(function(tests, num) {
    fs.readdirSync(path.join(testData, num)).forEach(function(testId) {
        var folder = path.join(testData, num, testId);

        if (fs.statSync(folder).isDirectory()) {
            var test = {
                data: path.join(folder, 'data.osm'),
                valid: fs.readFileSync(path.join(folder, 'result'), 'utf8').trim() === 'valid',
                info: require(path.join(folder, 'test.json')),
                wkt: {},
                geojson: {}
            };

            var re = /(\d+) (\w+\(.+\))/;
            ['nodes.wkt', 'ways.wkt'].forEach(function(filename) {
                var file = path.join(folder, filename);
                if (!fs.existsSync(file)) return;

                var data = fs.readFileSync(file, 'utf8').trim().split('\n').forEach(function(line) {
                    var m = line.match(re);
                    test.wkt[m[1]] = m[2];
                    test.geojson[m[1]] = wellknown(m[2]);
                });
            });

            tests.push(test);
        }
    });

    return tests;
}, []);


tests.forEach(function(test) {
    // use node-osmium to parse the data, create wkt and geojson
    var file = new osmium.File(test.data, 'osm');
    var handler = new osmium.Handler();
    var locator = new osmium.LocationHandler();
    var reader = new osmium.FlexReader(file, locator);
    var features = { wkt: {}, geojson: {} };
    var areas = { wkt: {}, geojson: {} };

    function getGeometry(osm) {
        var stash = osm.type === 'area' ? areas : features;
        try { stash.geojson[osm.id] = osm.geojson(); }
        catch (err) { stash.geojson[osm.id] = err; }
        try { stash.wkt[osm.id] = osm.wkt(); }
        catch (err) { stash.wkt[osm.id] = err; }
    }

    handler.on('node', getGeometry);
    handler.on('way', getGeometry);
    handler.on('area', getGeometry);
    osmium.apply(reader, handler);

    handler.end();
    reader.close();

    describe(test.info.test_id + ': ' + test.info.description, function() {
        // test basic feature wkt generation
        Object.keys(features.wkt).forEach(function(id) {
            var expected = test.wkt[id];
            var found = features.wkt[id];
            var errored = found instanceof Error;

            if (errored && !test.valid) {
                it('wkt threw error for invalid feature ' + id, function() {
                    assert.ok(true);
                });
            } else {
                it('generated wkt for feature ' + id, function() {
                    assert.equal(found, expected);
                });
            }

            delete test.wkt[id];
        });

        it('all expected wkt', function() {
            assert.deepEqual(Object.keys(test.wkt), []);
        });

        // test basic feature geojson generation
        Object.keys(features.geojson).forEach(function(id) {
            var expected = test.geojson[id];
            var found = features.geojson[id];
            var errored = found instanceof Error;

            if (errored && !test.valid) {
                it('geojson threw error for invalid feature ' + id, function() {
                    assert.ok(true);
                });
            } else {
                it('generated geojson for feature ' + id, function() {
                    assert.equal(JSON.stringify(found), JSON.stringify(expected));
                });
            }

            delete test.geojson[id];
        });

        it('all expected geojson', function() {
            assert.deepEqual(Object.keys(test.geojson), []);
        });

        if (!test.info.areas) return;

        // test area generation
        if (test.valid) {
            test.info.areas.default.forEach(function(expectedArea) {
                // osmium doubles id values when creating an area from a way and
                // doubles + 1 when creating an area from a relation
                var foundWkt = areas.wkt[2 * expectedArea.from_id] || areas.wkt[(2 * expectedArea.from_id) + 1];
                var foundGeojson = areas.geojson[2 * expectedArea.from_id] || areas.geojson[(2 * expectedArea.from_id) + 1];
                var expectedGeojson = wellknown(expectedArea.wkt);

                it('generated wkt area for feature ' + expectedArea.from_id, function() {
                    var equal = areasEqual(wellknown(foundWkt), expectedGeojson);
                    if (equal) assert.ok(equal);
                    else assert.equal(foundWkt, expectedArea.wkt);
                });
                it('generated geojson area for feature ' + expectedArea.from_id, function() {
                    var equal = areasEqual(foundGeojson, expectedGeojson);
                    if (equal) assert.ok(equal);
                    else assert.equal(JSON.stringify(foundGeojson), JSON.stringify(expectedGeojson));
                });
            });
        } else {
            (test.info.areas.fix || test.info.areas.location || test.info.areas.default || []).forEach(function(fixedArea) {
                var expected = fixedArea.wkt;
                var foundWkt = areas.wkt[2 * fixedArea.from_id] || areas.wkt[(2 * fixedArea.from_id) + 1];
                var foundGeojson = areas.geojson[2 * fixedArea.from_id] || areas.geojson[(2 * fixedArea.from_id) + 1];
                var erroredWkt = foundWkt instanceof Error;
                var erroredGeojson = foundGeojson instanceof Error;

                if (erroredWkt) {
                    it('wkt threw error for invalid area ' + fixedArea.from_id, function() {
                        assert.ok(true);
                    });
                } else {
                    it('generated fixed wkt for invalid area ' + fixedArea.from_id, function() {
                        var equal = areasEqual(wellknown(foundWkt), wellknown(expected));
                        if (equal) assert.ok(equal);
                        else assert.equal(foundWkt, expected);
                    });
                }

                if (erroredGeojson) {
                    it('geojson threw error for invalid area ' + fixedArea.from_id, function() {
                        assert.ok(true);
                    });
                } else {
                    it('generated fixed geojson for invalid area ' + fixedArea.from_id, function() {
                        var equal = areasEqual(foundGeojson, wellknown(expected));
                        if (equal) assert.ok(equal);
                        else assert.equal(JSON.stringify(foundGeojson), JSON.stringify(wellknown(expected)));
                    });
                }

                delete areas.wkt[2 * fixedArea.from_id];
                delete areas.wkt[2 * fixedArea.from_id + 1];
                delete areas.geojson[2 * fixedArea.from_id];
                delete areas.geojson[2 * fixedArea.from_id + 1];
            });

            Object.keys(areas.wkt).forEach(function(id) {
                var leftoverArea = areas.wkt[id];
                it('wkt threw error for invalid area ' + id, function() {
                    assert.ok(leftoverArea instanceof Error);
                });
            });

            Object.keys(areas.geojson).forEach(function(id) {
                var leftoverArea = areas.geojson[id];
                it('geojson threw error for invalid area ' + id, function() {
                    assert.ok(leftoverArea instanceof Error);
                });
            });
        }
    });
});
