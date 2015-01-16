#!/usr/bin/env node

var osmium = require('../../');

/*function ffunc() {
    console.log("ffunc");
}

setInterval(ffunc, 1000);*/

var file = new osmium.File('../../test/data/winthrop.osm');
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

