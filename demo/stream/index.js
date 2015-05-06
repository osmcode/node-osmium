#!/usr/bin/env node

var osmium = require('../../');

if (process.argv.length != 3) {
    console.log("Usage: " + process.argv[0] + ' ' + process.argv[1] + " OSMFILE");
    process.exit(1);
}

var input_filename = process.argv[2];

/*function ffunc() {
    console.log("ffunc");
}

setInterval(ffunc, 1000);*/

var file = new osmium.File(input_filename);
var location_handler = new osmium.LocationHandler();
var stream = new osmium.Stream(new osmium.Reader(file, location_handler));

var counts = { node: 0, way: 0, relation: 0, other: 0 };
stream.on('data', function(object) {
    if (counts.hasOwnProperty(object.type)) {
        counts[object.type]++;
    } else {
        counts.other++;
    }
});

stream.on('end', function() {
    console.log(counts.node);
    console.log(counts.way);
    console.log(counts.relation);
});

