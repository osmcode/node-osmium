var osmium = require('../');
var assert = require('assert');

var file = new osmium.File(__dirname + "/data/winthrop.osm");
var reader;

var counter = {};
function count(object) {
    counter[object.type]++;
}

describe('stream filters', function() {

    beforeEach(function() {
        counter.node = 0;
        counter.way = 0;
        counter.relation = 0;
        reader = new osmium.BasicReader(file);
    });

    it('should work without a filter', function(done) {
        var stream = new osmium.Stream(reader);

        stream.on('end', function() {
            assert.deepEqual(counter, { node: 1525, way: 98, relation: 2 });
            done();
        });

        stream.on('data', count);
    });

    it('should filter everything if empty', function(done) {
        var filter = new osmium.Filter();
        var stream = new osmium.Stream(reader, filter);

        stream.on('end', function() {
            assert.deepEqual(counter, { node: 0, way: 0, relation: 0 });
            done();
        });

        stream.on('data', count);
    });

    it('should allow everything with a filter that allows everything', function(done) {
        var filter = new osmium.Filter();
        filter.with_nodes().with_ways().with_relations();
        var stream = new osmium.Stream(reader, filter);

        stream.on('end', function() {
            assert.deepEqual(counter, { node: 1525, way: 98, relation: 2 });
            done();
        });

        stream.on('data', count);
    });

    it('should allow relations with a filter that allows relations', function(done) {
        var filter = new osmium.Filter();
        filter.with_relations();
        var stream = new osmium.Stream(reader, filter);

        stream.on('end', function() {
            assert.deepEqual(counter, { node: 0, way: 0, relation: 2 });
            done();
        });

        stream.on('data', count);
    });

    it('should work with keys', function(done) {
        var filter = new osmium.Filter();
        filter.with_nodes('amenity');
        var stream = new osmium.Stream(reader, filter);

        stream.on('end', function() {
            assert.deepEqual(counter, { node: 6, way: 0, relation: 0 });
            done();
        });

        stream.on('data', count);
    });

    it('should work with key-value combinations', function(done) {
        var filter = new osmium.Filter();
        filter.with_relations('type', 'boundary');
        var stream = new osmium.Stream(reader, filter);

        stream.on('end', function() {
            assert.deepEqual(counter, { node: 0, way: 0, relation: 1 });
            done();
        });

        stream.on('data', count);
    });

    it('should work with anything tagged', function(done) {
        var filter = new osmium.Filter();
        filter.with_nodes(null);
        var stream = new osmium.Stream(reader, filter);

        stream.on('end', function() {
            assert.deepEqual(counter, { node: 15, way: 0, relation: 0 });
            done();
        });

        stream.on('data', count);
    });

});

