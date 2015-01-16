var osmium = require('../');
var fs = require('fs');
var assert = require('assert');

describe('reader', function() {

    it('should throw when BasicReader called as function', function() {
        assert.throws(function() {
            var reader = osmium.BasicReader("foo");
        }, Error);
    });

    it('should not hang when apply() is called twice on reader', function() {
        var file = new osmium.File(__dirname + "/data/winthrop.osm");
        var reader = new osmium.BasicReader(file);
        var handler = new osmium.Handler();
        osmium.apply(reader, handler);

        assert.throws(function() {
            osmium.apply(reader, handler);
        }, Error);
    });

    it('should be able to create an osmium.BasicReader and access header', function() {
        var file = new osmium.File(__dirname + "/data/winthrop.osm");
        var reader = new osmium.BasicReader(file, {});
        var header = reader.header();
        assert.equal(header.generator, 'CGImap 0.2.0');

        var bounds = header.bounds[0];
        assert.ok(Math.abs(bounds.left()   - (-120.2024)) < .000000001);
        assert.ok(Math.abs(bounds.bottom() - (  48.4636)) < .000000001);
        assert.ok(Math.abs(bounds.right()  - (-120.1569)) < .000000001);
        assert.ok(Math.abs(bounds.top()    - (  48.4869)) < .000000001);
        reader.close();
    });

    it('should be able to call apply() with an osmium.BasicReader and a handler', function() {
        var reader = new osmium.BasicReader(__dirname + "/data/winthrop.osm");
        var handler = new osmium.Handler();

        osmium.apply(reader, handler);
    });

    it('should be able to create osmium.File with node.Buffer and read from it', function(done) {
        var buffer = fs.readFileSync(__dirname + "/data/winthrop.osm");
        assert.equal(buffer.length, 359898);

        var file = new osmium.File(buffer, "osm");
        assert.ok(file);

        var reader = new osmium.BasicReader(file);
        assert.ok(reader);

        var handler = new osmium.Handler();

        var count = 0;
        handler.on('node', function(node) {
            if (count++ == 0) {
                assert.equal(node.id, 50031066);
                done();
            }
        });

        osmium.apply(reader, handler);
    });

    it('should read a whole file with BasicReader.read_all()', function(done) {
        var file = new osmium.File(__dirname + "/data/winthrop.osm");
        var reader = new osmium.BasicReader(file);
        var buffer = reader.read_all();

        var handler = new osmium.Handler();

        var count = 0;
        handler.on('node', function(node) {
            if (count++ == 0) {
                assert.equal(node.id, 50031066);
                done();
            }
        });

        osmium.apply(buffer, handler);
    });

});

