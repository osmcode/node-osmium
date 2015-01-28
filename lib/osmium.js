
var util   = require('util'),
    stream = require('stream'),
    osmium = require('./binding/osmium.node');

exports = module.exports = osmium;
exports.version = require('../package').version;

exports.OSMObject.prototype.timestamp = function() {
    return new Date(1000 * this.timestamp_seconds_since_epoch);
}

exports.Changeset.prototype.created_at = function() {
    return new Date(1000 * this.created_at_seconds_since_epoch);
}

exports.Changeset.prototype.closed_at = function() {
    if (this.closed_at_seconds_since_epoch == 0) {
        return undefined;
    }
    return new Date(1000 * this.closed_at_seconds_since_epoch);
}

exports.Coordinates = function(lon, lat) {
    this.lon = lon;
    this.lat = lat;
    this.valid = function() {
        return this.lon !== undefined &&
               this.lat !== undefined &&
               this.lon >= -180.0 &&
               this.lon <= 180.0 &&
               this.lat >= -90.0 &&
               this.lat <= 90.0;
    };
}

exports.Box = function(bottom_left, top_right) {
    this.bottom_left = bottom_left;
    this.top_right = top_right;

    this.left   = function() { return this.bottom_left === undefined ? undefined : this.bottom_left.lon; };
    this.bottom = function() { return this.bottom_left === undefined ? undefined : this.bottom_left.lat; };
    this.right  = function() { return this.top_right   === undefined ? undefined : this.top_right.lon; };
    this.top    = function() { return this.top_right   === undefined ? undefined : this.top_right.lat; };
}

exports.Node.prototype.geojson = function() {
    return {
        type: 'Point',
        coordinates: [this.lon, this.lat]
    };
}

exports.Way.prototype.geojson = function() {
    return {
        type: 'LineString',
        coordinates: this.node_coordinates().map(function(c) {
            return [ c.lon, c.lat ];
        })
    };
}

// =================================================================

exports.Reader = function(file) {
    if (typeof(file) == 'string') {
        file = new osmium.File(file);
    }
    this._file = file;

    if (arguments.length == 2) {
        if (typeof(arguments[1]) == 'string' || arguments[1] instanceof osmium.LocationHandler) {
            var location_handler = arguments[1];
            this._location_handler = location_handler;
            this._reader = new osmium.FlexReader(file, location_handler);
        } else {
            var options = arguments[1];
            this._reader = new osmium.BasicReader(file, options);
        }
    } else if (arguments.length == 3) {
        var location_handler = arguments[1];
        var options = arguments[2];
        this._location_handler = location_handler;
        this._reader = new osmium.FlexReader(file, location_handler, options);
    } else {
        throw new Error('Wrong number of arguments for Reader');
    }
}

exports.Reader.prototype.read = function() {
    return this._reader.read();
}

exports.Reader.prototype.close = function() {
    return this._reader.close();
}

// =================================================================

exports.Filter = function() {
    this._node      = [];
    this._way       = [];
    this._relation  = [];
    this._changeset = [];
    this._area      = [];
}

exports.Filter.prototype.with_nodes = function(key, value) {
    this._node.push([key, value]);
    return this;
}

exports.Filter.prototype.with_ways = function(key, value) {
    this._way.push([key, value]);
    return this;
}

exports.Filter.prototype.with_relations = function(key, value) {
    this._relation.push([key, value]);
    return this;
}

exports.Filter.prototype.with_changesets = function(key, value) {
    this._changeset.push([key, value]);
    return this;
}

exports.Filter.prototype.with_areas = function(key, value) {
    this._area.push([key, value]);
    return this;
}

// =================================================================

var DummySource = function() {
}

DummySource.prototype.read = function() {
    return undefined;
};

exports.Stream = function(source, filter) {
    stream.Readable.call(this, { objectMode: true });

    if (filter) {
        this._filter = filter;
        this._filter_id = osmium.register_filter(filter);
    } else {
        this._filter_id = 0;
    }

    if (source instanceof osmium.Reader || source instanceof osmium.BasicReader || source instanceof osmium.FlexReader) {
        this._source = source;
        this._buffer = source.read();
    } else if (source instanceof osmium.Buffer) {
        this._source = new DummySource();
        this._buffer = source;
    } else {
        throw new Error('osmium.Stream must be created with osmium.[Basic|Flex]Reader or osmium.Buffer as argument');
    }

    this._callbacks = {};
}

util.inherits(exports.Stream, stream.Readable);

exports.Stream.prototype._read_object = function() {
    // eof already reached?
    if (this._buffer === undefined) {
        this.push(null);
        return;
    }

    var object = this._buffer.next(this._filter_id);

    if (object === undefined) {
        this._buffer = this._source.read();
        if (this._buffer === undefined) {
            this.push(null);
            return;
        }
        object = this._buffer.next(this._filter_id);
    }

    this.push(object);
}

exports.Stream.prototype._read = function() {
    setImmediate(this._read_object.bind(this));
}

exports.Stream.prototype.set_callback = function(type, func) {
    this._callbacks[type] = func;
}

exports.Stream.prototype.dispatch = function(data) {
    var callback = this._callbacks[data.type];
    if (callback) {
        return callback(data);
    }
}

// =================================================================

exports.Converter = function(args) {
    if (args === undefined) {
        args = {};
    }

    this.show_layers = args.show_layers ? true : false;
    this.output_name = args.output || "out.db";

    function Layer(name, geom_type) {
        this.name = name;
        this.geom_type = geom_type.toUpperCase();

        if (this.geom_type != 'POINT' && this.geom_type != 'LINESTRING' && this.geom_type != 'MULTIPOLYGON') {
            throw new Error('Unknown geometry type: "' + geom_type + '" (allowed are: "POINT", "LINESTRING", and "MULTIPOLYGON")');
        }

        this.attributes = [];
        this.attribute_names = {};

        this.with_attribute = function(name, type) {
            var attribute_type = type.toUpperCase();
            if (attribute_type != 'INTEGER' && attribute_type != 'STRING' && attribute_type != 'BOOL' && attribute_type != 'REAL') {
                throw new Error('Unknown attribute type: ' + type);
            }
            var attribute = { name: name, type: attribute_type };
            this.attributes.push(attribute);
            this.attribute_names[name] = attribute;
            return this;
        }
    }

    var layers = {};

    function Filter(func) {
        this.match = func;
    }

    function KeyValueFilter(key, value) {
        this.key = key;

        if (value == '*') {
            this.value = null;
        } else {
            this.value = value;
        }

        if (typeof(value) == 'string' && value.match(/\|/)) {
            value = value.split('|');
        }

        if (value == null) {
            this.match = function(obj) {
                return !!obj.tags(key);
            };
        } else if (typeof(value) == 'string') {
            this.match = function(obj) {
                return obj.tags(key) == value;
            };
        } else if (value instanceof Array) {
            this.match = function(obj) {
                var val = obj.tags(key);
                if (! val) {
                    return false;
                }
                return value.indexOf(val) != -1;
            };
        } else if (value instanceof RegExp) {
            this.match = function(obj) {
                return obj.tags(key) && value.test(obj.tags(key));
            }
        } else {
            throw new Error('Can not understand rule');
        }
    }

    function Rule(type) {
        this.type = type;

        this.layer = null;
        this.filters = [];
        this.attrs = {};

        this.matching = function(key, value) {
            if (key instanceof Function) {
                this.filters.push(new Filter(key));
            } else {
                this.filters.push(new KeyValueFilter(key, value));
            }
            return this;
        };

        this.to_layer = function(name) {
            if (! layers[name]) {
                throw new Error('Unknown layer: ' + name);
            }
            this.layer = name;
            return this;
        };

        this.with_attribute = function(attr, key) {
            if (this.layer == null) {
                throw new Error('Layer not set for rule ' + key + '=' + value);
            }

            if (! layers[this.layer].attribute_names[attr]) {
                throw new Error("There is no attribute named '" + attr + "' in layer '" + this.layer + "'");
            }

            if (key instanceof Function) {
                this.attrs[attr] = key;
            } else if (key == null) {
                this.attrs[attr] = function(tags) {
                    return tags[attr];
                };
            } else {
                this.attrs[attr] = function(tags) {
                    return tags[key];
                };
            }

            return this;
        };

        this.match = function(osm_object) {
            var result = true;
            this.filters.forEach(function(filter) {
                if (!filter.match(osm_object)) {
                    result = false;
                }
            });
            return result;
        };
    }

    this.create_layer = function(name, geom_type) {
        if (geom_type == undefined) {
            geom_type = 'POINT';
        }
        var layer = new Layer(name, geom_type);
        layers[name] = layer;
        return layer;
    }

    var rules = {
        node: [],
        way:  [],
        area: []
    };

    this.rule = function(object_type) {
        var rule = new Rule(object_type);
        rules[object_type].push(rule);
        return rule;
    }

    this.add_nodes = function() {
        return this.rule('node');
    }

    this.add_ways = function() {
        return this.rule('way');
    }

    this.add_areas = function() {
        return this.rule('area');
    }

    function tags2attributes(id, tags, attrs) {
        var obj = { osm_id: id };
        for (var a in attrs) {
            obj[a] = attrs[a](tags);
        }
        return obj;
    }

    this.convert = function(input) {

        var fs = require('fs');
        if (fs.existsSync(this.output_name)) {
            console.log("Output database file '" + this.output_name + "' exists. Refusing to overwrite it.");
            process.exit(1);
        }

        // setup database
        var sqlite = require('spatialite');
        var db = new sqlite.Database(this.output_name);
        db.serialize();

        db.spatialite();
        db.run("PRAGMA synchronous = OFF;"); // otherwise it is very slow
        db.run("SELECT InitSpatialMetaData('WGS84');");

        var show_layers = this.show_layers;

        // setup layers in database
        for (var t in layers) {
            var layer = layers[t];

            if (show_layers) {
                console.log('Layer: ' + layer.name + ' (' + layer.geom_type + ')');
            }

            db.run("CREATE TABLE " + layer.name + " (id INTEGER PRIMARY KEY);");
            db.run("SELECT AddGeometryColumn('" + layer.name + "', 'geom', 4326, '" + layer.geom_type + "', 2);");

            var insert = "INSERT INTO " + layer.name + " (geom";
            var qm = '';

            layer.attributes.forEach(function(attribute) {
                if (show_layers) {
                    console.log('  ' + (attribute.name + '          ').substr(0, 11) + attribute.type);
                }
                db.run("ALTER TABLE " + layer.name + " ADD COLUMN " + attribute.name + " " + attribute.type + ";");
                insert += ", " + attribute.name;
                qm += ", ?";
            });

            insert += ") VALUES (GeomFromWKB(?, 4326)" + qm + ");";

            layer.insert = db.prepare(insert);
            if (show_layers) {
                console.log('');
            }
        }

        // convert data
        console.log("Converting data...");
        var options = { node: true, way: true };
        if (rules.area.length > 0) {
            options.area = true;
        }

        var location_handler = new osmium.LocationHandler();
        var reader = new osmium.Reader(input, location_handler, options);
        var stream = new osmium.Stream(reader);

        stream.on('data', function(osm_object) {
            rules[osm_object.type].forEach(function(rule) {
                if (rule.match(osm_object)) {
                    try {
                        var attributes = tags2attributes(osm_object.id, osm_object.tags(), rule.attrs);
                        var p = [osm_object.wkb()];
                        var layer = layers[rule.layer];
                        layer.attributes.forEach(function(attribute) {
                            p.push(attributes[attribute.name]);
                        });
                        layer.insert.run.apply(layer.insert, p);
                    } catch (e) {
                        // ignore broken geometries
                    }
                }
            });
        });

        stream.on('end', function() {
            console.log("Cleanup...");
            for (var layer in layers) {
                layers[layer].insert.finalize();
            }

            db.close(function() {
                console.log("Done.");
            });
        });
    };

};

