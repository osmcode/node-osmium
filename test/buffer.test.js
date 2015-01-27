var osmium = require('../');
var fs = require('fs');
var assert = require('assert');

describe('buffer', function() {

    it('should allow reading into buffer', function(done) {
        var file = new osmium.File(__dirname + "/data/winthrop.osm");
        var reader = new osmium.BasicReader(file);
        var buffer = reader.read();
        var stream = new osmium.Stream(buffer);

        var count = 0;
        stream.set_callback('node', function(node) {
            if (count++ == 0) {
                assert.equal(node.id, 50031066);
                done();
            }
        });

        stream.on('data', stream.dispatch);
    });

    it('should allow reading into buffer in a loop', function(done) {
        var file = new osmium.File(__dirname + "/data/winthrop.osm");
        var reader = new osmium.BasicReader(file);

        var count = 0;
        function node_callback(node) {
            if (count++ == 0) {
                assert.equal(node.id, 50031066);
                done();
            }
        }

        var buffer;
        while (buffer = reader.read()) {
            var stream = new osmium.Stream(buffer);
            stream.set_callback('node', node_callback);
            stream.on('data', stream.dispatch);
        }
    });

    it('should allow iterating over buffer', function() {
        var file = new osmium.File(__dirname + "/data/winthrop.osm");
        var reader = new osmium.BasicReader(file);

        var buffer;
        while (buffer = reader.read()) {
            var object = buffer.next();
            assert.ok(object instanceof osmium.Node);
            assert.equal(object.id, 50031066);

            object = buffer.next();
            assert.ok(object instanceof osmium.Node);
            assert.equal(object.id, 50031085);

            var count=0, ways=0;
            while (object = buffer.next()) {
                if (ways == 0 && object instanceof osmium.Way) {
                    ++ways;
                    assert.equal(object.id, 6091729);
                }
                ++count;
            }
            assert.equal(count, 1525 /*nodes*/ + 98 /*ways*/ + 2 /*relations*/ - 2 /*already read*/);
        }
    });

    it('should be able to stream buffer created from a file', function(done) {
        var count = 0;
        var data = fs.readFileSync(__dirname + "/data/winthrop.osm.ser");
        var buffer = new osmium.Buffer(data);
        var stream = new osmium.Stream(buffer);

        stream.set_callback('node', function(node) {
            if (count++ == 0) {
                assert.equal(node.id, 50031066);
                assert.equal(node.visible, true);
                assert.equal(node.version, 2);
                assert.equal(node.changeset, 3137735);
                assert.equal(node.user, "woodpeck_fixbot");
                assert.equal(node.uid, 147510);
                assert.equal(node.timestamp_seconds_since_epoch, 1258416656);
                assert.equal(node.timestamp().toISOString(), '2009-11-17T00:10:56.000Z');
                assert.equal(node.coordinates.lon, -120.1891610);
                assert.equal(node.coordinates.lat, 48.4655800);
                assert.equal(node.lon, -120.1891610);
                assert.equal(node.lat, 48.4655800);
                done();
            }
        });

        stream.on('data', stream.dispatch);
    });

    it('should complain when calling Buffer methods on something else', function() {
        var data = fs.readFileSync(__dirname + "/data/winthrop.osm.ser");
        var buffer = new osmium.Buffer(data);

        assert.throws(function() {
            buffer.clear.apply(undefined);
        }, TypeError);

        assert.throws(function() {
            buffer.clear.apply(data);
        }, TypeError);
    });

});

