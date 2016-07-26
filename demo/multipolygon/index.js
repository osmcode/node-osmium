#!/usr/bin/env node

var osmium = require('../../');
var buffered_writer = require('buffered-writer');

if (process.argv.length != 4) {
    console.log("Usage: " + process.argv[0] + ' ' + process.argv[1] + " OSMFILE OUTFILE");
    process.exit(1);
}

var input_filename = process.argv[2];
var output_filename = process.argv[3];

var stream = buffered_writer.open(output_filename);

stream.on("error", function(error) {
    console.log(error);
})

// =====================================

var handler = new osmium.Handler();

handler.on('area', function(area) {
    var landuse = area.tags('landuse');
    if (landuse) {
        try {
            stream.write('a' + area.id + ' ' + landuse + ' ' + (area.tags('name') || '') + ' ' +  area.wkt() + "\n");
        } catch(e) {
            // ignore errors
        }
    }
});

handler.on('done', function() {
    stream.close();
});

var location_handler = new osmium.LocationHandler();

// using the FlexReader interface...
var reader = new osmium.FlexReader(input_filename, location_handler);
osmium.apply(reader, handler);
handler.end();
reader.close();

// ...or using the old "manual" interface...
/*
var reader = new osmium.BasicReader(input_filename);
var mp = new osmium.MultipolygonCollector();
mp.read_relations(reader);
reader.close();

reader = new osmium.BasicReader(input_filename);
osmium.apply(reader, location_handler, handler, mp.handler(handler));
mp.handler(handler).end();
handler.end();
reader.close();
*/

