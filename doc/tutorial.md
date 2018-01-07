
# Osmium Node Module Tutorial

## Using the node-osmium Library

Install with

```javascript
npm install osmium
```

and add

```javascript
var osmium = require('osmium');
```

in your Javascript file to access it.

## OSM File Access

`node-osmium` can read all the popular types of OSM files. It can read the XML
format (without compression (`.osm`) or with gzip (`.osm.gz`) or bzip2
(`.osm.bz2`) compression), the PBF format (`.osm.pbf`), files with (`.osh`) or
without (`.osm`) historic data and change files (`.osc`). It can also read OSM
files with changesets.

To open such a file you need a `File` object:

```javascript
var file = new osmium.File("some_file.osm");
```

Osmium will detect the file format from the filename suffix. It understands all
the usual suffixes. If it doesn't understand the suffix, you can add a second
parameter with the format. For instance the following example will force the
uncompressed XML format:

```javascript
var file = new osmium.File("some_file", "osm");
```

This will force PBF:

```javascript
var file = new osmium.File("some_file", "pbf");
```

You can read from STDIN, but you have to set the format then:

```javascript
var file = new osmium.File("-", "pbf");
```

You can also read directly from a URL like this:

```javascript
var file = new osmium.File("http://example.com/data.osm", "osm");
```

### Accessing OSM Data From a Node Buffer

Sometimes you have OSM data not in a file on disk but somewhere else. If you
can get it into a `node.Buffer` you can get it into Osmium. Simply give the
buffer to the `osmium.File` constructor instead of a file name:

```javascript
var buffer = new node.Buffer();
// fill buffer with data
var file = osmium.File(buffer, "pbf"); // buffer contents in PBF format
```

## Creating a Reader

Once you have defined a `File` you can open a `Reader` to access its data:

```javascript
var file = new osmium.File("data.osm");
var reader = new osmium.Reader(file);
```

In this simple case you can also directly open the `Reader` with a file name:

```javascript
var reader = new osmium.Reader("data.osm");
```

But if you need to specify a format or want to read from a `node.Buffer` you
have to initialize the `osmium.File` first.

### Defining Which Object Types to Read

OSM files usually contain nodes, ways, and/or relations. Some special OSM files
can contain changelog entries. If you only want to read some of those object
types, you can specify which by adding an extra object parameter when
initializing the Reader.

In case you are only interested in nodes and ways it would look like this:

```javascript
var reader = new osmium.Reader("data.osm", { node: true, way: true });
```

The default is to read all the different object types. But to improve
performance it is recommened to only read the object types you really need.
That way other object types are discarded on the C++ side avoiding the costly
C++-to-Javascript conversion.

### Getting the File Header

The `Reader` object gives you access to the OSM file header:

```javascript
var reader = new osmium.Reader("data.osm");
var header = reader.header();
```

The `header` object contains the `generator`, the software that created this
file:

```javascript
header.generator; // ===> 'CGImap 0.2.0'
```

And it contains the bounding box(es) of the data. (OSM files can have zero
or more bounding boxes, usually they have one.)

```javascript
var bounds = header.bound[0];
var min_lon = bounds.left();
var max_lon = bounds.right();
var min_lat = bounds.bottom();
var max_lat = bounds.top();
```

## Defining a Handler

Osmium will generate events for each object it reads. Those events can be
handled by your code. For this you need to define a handler and define some
callbacks on it:

```javascript
var handler = new osmium.Handler();
handler.on('node', function(node) {
    console.log("got a node with id ", node.id);
});
```

There are callbacks like the above for each type of OSM object: `node`, `way`,
`relation`, and `changeset`.

Sometimes you need to run initialization or finalization code. Use the `init`
and `done` callbacks:

```javascript
handler.on('init', function() { ... });
handler.on('done', function() { ... });
```

There are also "before" and "after" callbacks called between objects of
different types:

```javascript
handler.on('before_nodes', function() { ... });
handler.on('after_nodes', function() { ... });
handler.on('before_ways', function() { ... });
...
```

### Handler Options

Sometimes you are only interested in nodes with tags. But the majority of
nodes do not have any tags at all. As an optimization you can tell the
handler to only give you tagged nodes:

```javascript
var handler = new osmium.Handler();
handler.options({ 'tagged_nodes_only': true });
```

## Accessing OSM Objects

Through the `node`, `way` and `relation` callbacks you get access to the
OSM objects.

OSM objects are read-only and you can not create them yourself. Think of
them not as real objects, but convenient accessors to the data in the OSM
file. You can not keep the objects around outside the handler callback.
If you need to retain some data from the objects, extract the data you need
and put it in your own data structure. OSM files can be quite large, so
make sure you'll store the data efficiently.

### Accessing OSM Object Attributes

Each object has the usual attributes:

```javascript
var handler = new osmium.Handler();
handler.on('node', function(node) {
    console.log(node.id);        // unique id of this object
    console.log(node.version);   // version of this object
    console.log(node.changeset); // changeset id of this object
    console.log(node.uid);       // user id
    console.log(node.user);      // name of user
});
```

Accessing the timestamp when this object version was created is a bit more
complex. To get a `Date` object with this information call the `timestamp()`
function:

```javascript
    console.log(node.timestamp());
```

Because this is an expensive operation and often a full `Date` object is not
needed, you can also get the timestamp as a simple number counting the seconds
since midnight January 1, 1970:

```javascript
    console.log(node.timestamp_seconds_since_epoch);
```

The `node.visible` attribute tells you whether an object is visible or has
been deleted. For normal OSM files it is always true, but if the file contains
old versions of the OSM data ("history file") or is a "change file" (`.osc`),
this attribute can be true or false.

The same works for ways and relations. Changelog objects are different and not
documented here.

### Accessing the Tags

Of course you can also get access to the tags:

```javascript
way.tags(); // ==> { "highway": "residential", "maxspeed": "50" }
```

or ask for a specific key:

```javascript
way.tags("highway"); // ==> "residential"
```

Use the second form if you are only interested in a few tags, because it is
faster.

### Accessing Node Locations And the Coordinates Property

OSM node objects contain the location of the node, the coordinates. You can
access them in several ways:

```javascript
node.lon;           // ==> Number between -180.0 and 180.0
node.lat;           // ==> Number between -90.0 and 90.0
node.coordinates;   // ==> Coordinates object
```

The `Coordinates` object returned by the `coordinates` property has `lon`
and `lat` properties:

```javascript
var c = node.coordinates;
console.log(c.lon);
console.log(c.lat);
```

Unlike the `Node` object, the `Coordinates` object can created by you and it
can be copied around and used like any normal Javascript object.

### Accessing Way Nodes

Ways have a reference to the IDs of the nodes they use:

```javascript
way.node_refs();  // ==> [12345, 629375, 273054] (Array with node IDs)
way.node_refs(1); // ==> 629375  (ID of 2nd node)
```

You can not access a node object this way, only the ID. See below for accessing
the node locations.

If all you need is the number of referenced nodes, use the `nodes_count`
property of the way object:

```javascript
way.nodes_count;  // ==> number of nodes in this way
```

### Accessing Relation Members

Relations have data about their members. To access them use the `members()`
function:

```javascript
relation.members();   // ==> Array of members
relation.members(2);  // ==> Third member
```

A member is represented as an Object with three keys:

1. `type` - te type of the member ('n', 'w', or 'r')
2. `ref` - the ID of the member
3. `role` - the role of the member

If all you need is the number of members, use the `members_count` property of
the relation object:

```javascript
relation.members_count;  // ==> number of members in this way
```

## The LocationHandler

OSM ways contain only references to the nodes, but in most cases you don't need
the ID but the location. This is where the `LocationHandler` comes into play.
Initialize and call the `LocationHandler` like this:

```javascript
var reader = new osmium.Reader("some_file.osm");
var location_handler = new osmium.LocationHandler();
var handler = new osmium.Handler();
// set up your handler callbacks
osmium.apply(reader, location_handler, handler);
```

This will call the location handler before your handler. When the location
handler encounters nodes, it will store their location. Later, reading the
same file it will "add" the locations to the ways so that your handler will
see ways with added node locations. You can access them like this:

```javascript
way.node_coordinates();
```

This will return an `Array` with `osmium.Coordinates` objects. It is also
the basis for the geometry functions described below.

The `LocationHandler` can use different strategies for storing the node
locations. Which strategy is the best depends on the size of the input file and
the amount of main memory you have. Here are some of the available options:

 * `sparse_mem_array` (default) - use this for small (city) to medium (country)
   sized data files.
 * `sparse_mmap_array` - a bit more efficient than `sparse_mem_array`, but only
   available on Linux.
 * `dense_mmap_array` - best memory efficiency for large countries and planet
   sized data files. You will need main memory of size (8 bytes times the
   highest node ID), for a planet thats currently on the order of 32 GByte main
   memory! Only available on Linux.
 * `sparse_file_array` (default) - use this for small (city) to medium (country)
   sized data files if you are short on memory.
 * `dense_file_array` - best memory efficiency for large countries and planet
   sized data files, but uses hard disk instead of memory. Use for very large
   data files if you are on OSX (and therefore can't use the "mmap" type) or
   if you don't have enough main memory. Will, of course, be slow compared to
   the other strategies.

To set the strategy initialize the `LocationHandler` with its name:

```javascript
var location_handler = new osmium.LocationHandler("dense_mmap_array");
```

## Geometry Functions

Usually you don't want (arrays of) raw coordinates, but you want geometries in
some standard format. Osmium supports WKT (Well Known Text), WKB (Well Known
Binary) and GeoJSON representations of geometries for nodes and ways. You have
to use a LocationHandler as described above for these functions to work for
ways.

### WKT

The `wkt()` function called on a node or way returns a `String` with a
representation of the geometry:

```javascript
node.wkt(); // ===> "POINT(1.56 9.20)"
way.wkt();  // ===> "LINESTRING(1.56 9.20, 4.56 10.29)"
```

See http://en.wikipedia.org/wiki/Well-known_text for a description of the
WKT and WKB format.

### WKB

The `wkb()` function called on a node or way returns a `node.Buffer` with a
binary representation of the geometry.

```javascript
node.wkb();
way.wkb();
```

See http://en.wikipedia.org/wiki/Well-known_text for a description of the
WKT and WKB format.

### GeoJSON

The `geojson` function called on a node or way returns an Object with a
representation of the geometry according to the GeoJSON spec:

```javascript
node.geojson();
```

will result in something like this:

```javascript
{
    type: 'Point',
    coordinates: [-120.1891610, 48.4655800]
}
```

and

```javascript
way.geojson();
```

will result in something like this:

```javascript
{
    type: 'LineString',
    coordinates: [
        [-120.1796227, 48.4798110],
        [-120.1787663, 48.4802976]
    ]
}
```

This is, of course, only the "geometry part" of a full feature, you have to
add the rest yourself. See the `demo/geojson-stream` example for a complete
program using the GeoJSON function.

## Working with Multipolygons

OSM doesn't have a data type for areas or polygons. Instead areas are stored
as closed ways (ie first node == last node) or relations tagged as
`type=multipolygon`. Osmium can hide this complexity and create pseudo-objects
called "Areass" that are either based on closed ways or on those multipoygon
relations. Usually OSM files have to be read twice to allow this, though. In
the first pass, relations are read and prepared, in the second pass nodes and
ways are read and everything is assembled. In addition to the `node`, `way`,
and `relation` callbacks, you can define an `area` callback.

Here is an example:

```javascript
var handler = new osmium.Handler();

handler.on('area', function(area) {
    var landuse = area.tags('landuse');
    if (landuse) {
        console.log(area.wkt() + ' ' + landuse);
    }
});

var mp = new osmium.MultipolygonCollector();

var reader = new osmium.BasicReader(input_filename);
mp.read_relations(reader);
reader.close();

reader = new osmium.Reader(input_filename);
var location_handler = new osmium.LocationHandler();
osmium.apply(reader, location_handler, handler, mp.handler(handler));
reader.close();
```

The code looks a bit complicated, because it is modelled after the C++ it is
based on. It is likely we'll make this easier at some point. You can not re-use
the reader from the first pass, you have to create a new one for the second
pass.


## Working With Buffers

Instead of calling `apply()` you can call `read()` on the Reader and you'll get
an `osmium.Buffer` with zero or more OSM entities in it. If there is no more
data in the file, `read()` returns `undefined`:

```javascript
var reader = new osmium.Reader("foo.osm");
var buffer;
while (buffer = reader.read()) {
    // do something here
}
```

You can call `apply()` with a `Buffer` and handlers, just like you would do
with the `Reader`:

```javascript
osmium.apply(buffer, handler1, handler2);
```

There are no guarantees how many entities are in one of those buffers. They can
even be empty, but usually they will contain a few thousand entities, so they
are "small" in comparison to a big OSM data file. So if you call `apply()` on a
`Buffer` it will return much sooner than when calling `apply()` on the
`Reader`. This allows you some amount of control over how fast the input is
read.

```javascript
var reader = new osmium.Reader("foo.osm");
var buffer;
while (buffer = reader.read()) {
    osmium.apply(buffer, handler1, handler2);
    // wait here if needed to slow down reading the file
}
```

Note that calling `apply()` repeatedly in this way on the buffers instead of
once on the `Reader`, confuses the `done`, `init`, and `before_*` and `after_*`
handlers. You will get those handlers called for each `apply()` separately.

### Iterating Over the Contents of a Buffer

Instead of calling `apply()` you can iterate over the contents of the buffer by
calling `next()`:

```javascript
var object;
while (object = buffer.next()) {
    console.log(object.id());
}
```

`buffer.next()` returns `undefined` when there is no more data.

If you use the `buffer.next()` call, you'll get the next object, whatever that
is, you have to check with `object instanceof osmium.Node` or so if you got the
right type. Note that you can not use the `LocationHandler` this way.

Together you can read the content of a file like this:

```javascript
var reader = new osmium.Reader("foo.osm");
var buffer;
while (buffer = reader.read()) {
    var object;
    while (object = buffer.next()) {
        // do something here with object
    }
}
```

### Creating an Osmium Buffer From a Node Buffer

You can also create an `osmium.Buffer` from a `node.Buffer`:

```javascript
var node_buffer = new node.Buffer;
// fill it with OSM data somehow
var osmium_buffer = new osmium.Buffer(node_buffer);
```

This buffer can now be used like the buffers we got from the `Reader`.

## A Complete Example

Finally here is a complete example to get you started: This parses an OSM file
and creates a node handler callback to count the total number of nodes:

```javascript
var osmium = require('osmium');
var reader = new osmium.Reader("test/data/winthrop.osm");
var handler = new osmium.Handler();
var nodes = 0;
handler.on('node', function(node) {
    ++nodes;
});
osmium.apply(reader, handler);
console.log(nodes);
```

Result:

    1525

