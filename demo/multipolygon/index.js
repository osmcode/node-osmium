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

// =====================================

var handler = new osmium.Handler();

handler.on('area', function(area) {
    var landuse = area.tags('landuse');
    if (landuse) {
        stream.write('a' + area.id + ' ' + landuse + ' ' + (area.tags('name') || '') + ' ' +  area.wkt() + "\n");
    }
});

handler.on('done', function() {
    stream.close();
});

var location_handler = new osmium.LocationHandler();
var reader = new osmium.FlexReader(input_filename, location_handler);
osmium.apply(reader, handler);
reader.close();

