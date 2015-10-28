
// node
#include <node.h>

// osmium
#include <osmium/geom/wkb.hpp>
#include <osmium/geom/wkt.hpp>
#include <osmium/geom/geojson.hpp>
#include <osmium/io/input_iterator.hpp>
#include <osmium/memory/buffer.hpp>
#include <osmium/visitor.hpp>

// node-osmium
#include "apply.hpp"
#include "buffer_wrap.hpp"
#include "file_wrap.hpp"
#include "filter.hpp"
#include "handler.hpp"
#include "location_handler_wrap.hpp"
#include "multipolygon_collector_wrap.hpp"
#include "multipolygon_handler_wrap.hpp"
#include "osm_area_wrap.hpp"
#include "osm_changeset_wrap.hpp"
#include "osm_node_wrap.hpp"
#include "osm_relation_wrap.hpp"
#include "osm_way_wrap.hpp"
#include "basic_reader_wrap.hpp"
#include "flex_reader_wrap.hpp"

namespace node_osmium {

    v8::Persistent<v8::Object> module;
    osmium::geom::WKBFactory<> wkb_factory;
    osmium::geom::WKTFactory<> wkt_factory;
    osmium::geom::GeoJSONFactory<> geojson_factory;

    v8::Persistent<v8::String> symbol_OSMEntity;
    v8::Persistent<v8::String> symbol_OSMObject;

    v8::Persistent<v8::String> symbol_Node;
    v8::Persistent<v8::String> symbol_node;

    v8::Persistent<v8::String> symbol_Way;
    v8::Persistent<v8::String> symbol_way;

    v8::Persistent<v8::String> symbol_Relation;
    v8::Persistent<v8::String> symbol_relation;
    v8::Persistent<v8::String> symbol_type;
    v8::Persistent<v8::String> symbol_ref;
    v8::Persistent<v8::String> symbol_role;

    v8::Persistent<v8::String> symbol_Area;
    v8::Persistent<v8::String> symbol_area;

    v8::Persistent<v8::String> symbol_Changeset;
    v8::Persistent<v8::String> symbol_changeset;

    v8::Persistent<v8::String> symbol_Coordinates;
    v8::Persistent<v8::String> symbol_Box;

    v8::Persistent<v8::String> symbol_generator;
    v8::Persistent<v8::String> symbol_bounds;

    v8::Persistent<v8::String> symbol_Buffer;
    v8::Persistent<v8::String> symbol_File;
    v8::Persistent<v8::String> symbol_Handler;
    v8::Persistent<v8::String> symbol_LocationHandler;
    v8::Persistent<v8::String> symbol_MultipolygonCollector;
    v8::Persistent<v8::String> symbol_MultipolygonHandler;
    v8::Persistent<v8::String> symbol_BasicReader;
    v8::Persistent<v8::String> symbol_FlexReader;

    v8::Persistent<v8::Object> the_Node;
    v8::Persistent<v8::Object> the_Way;
    v8::Persistent<v8::Object> the_Relation;
    v8::Persistent<v8::Object> the_Area;
    v8::Persistent<v8::Object> the_Changeset;

    extern "C" {
        static void start(v8::Handle<v8::Object> target) {
            v8::HandleScope scope;
            module = v8::Persistent<v8::Object>::New(target);

            node::SetMethod(target, "apply_", node_osmium::apply);
            node::SetMethod(target, "register_filter", node_osmium::Filter::register_filter);

            symbol_OSMEntity       = NODE_PSYMBOL("OSMEntity");
            symbol_OSMObject       = NODE_PSYMBOL("OSMObject");
            symbol_Node            = NODE_PSYMBOL("Node");
            symbol_node            = NODE_PSYMBOL("node");
            symbol_Way             = NODE_PSYMBOL("Way");
            symbol_way             = NODE_PSYMBOL("way");
            symbol_Relation        = NODE_PSYMBOL("Relation");
            symbol_relation        = NODE_PSYMBOL("relation");
            symbol_type            = NODE_PSYMBOL("type");
            symbol_ref             = NODE_PSYMBOL("ref");
            symbol_role            = NODE_PSYMBOL("role");
            symbol_Area            = NODE_PSYMBOL("Area");
            symbol_area            = NODE_PSYMBOL("area");
            symbol_Changeset       = NODE_PSYMBOL("Changeset");
            symbol_changeset       = NODE_PSYMBOL("changeset");
            symbol_Coordinates     = NODE_PSYMBOL("Coordinates");
            symbol_Box             = NODE_PSYMBOL("Box");
            symbol_generator       = NODE_PSYMBOL("generator");
            symbol_bounds          = NODE_PSYMBOL("bounds");

            symbol_Buffer                = NODE_PSYMBOL("Buffer");
            symbol_File                  = NODE_PSYMBOL("File");
            symbol_Handler               = NODE_PSYMBOL("Handler");
            symbol_LocationHandler       = NODE_PSYMBOL("LocationHandler");
            symbol_MultipolygonCollector = NODE_PSYMBOL("MultipolygonCollector");
            symbol_MultipolygonHandler   = NODE_PSYMBOL("MultipolygonHandler");
            symbol_BasicReader           = NODE_PSYMBOL("BasicReader");
            symbol_FlexReader            = NODE_PSYMBOL("FlexReader");

            node_osmium::OSMEntityWrap::Initialize(target);
            node_osmium::OSMObjectWrap::Initialize(target);
            node_osmium::OSMNodeWrap::Initialize(target);
            node_osmium::OSMWayWrap::Initialize(target);
            node_osmium::OSMRelationWrap::Initialize(target);
            node_osmium::OSMAreaWrap::Initialize(target);
            node_osmium::OSMChangesetWrap::Initialize(target);
            node_osmium::MultipolygonCollectorWrap::Initialize(target);
            node_osmium::MultipolygonHandlerWrap::Initialize(target);
            node_osmium::LocationHandlerWrap::Initialize(target);
            node_osmium::JSHandler::Initialize(target);
            node_osmium::FileWrap::Initialize(target);
            node_osmium::BasicReaderWrap::Initialize(target);
            node_osmium::FlexReaderWrap::Initialize(target);
            node_osmium::BufferWrap::Initialize(target);

            Filter::init_filters();

            the_Node      = v8::Persistent<v8::Object>::New(new_external<OSMNodeWrap>());
            the_Way       = v8::Persistent<v8::Object>::New(new_external<OSMWayWrap>());
            the_Relation  = v8::Persistent<v8::Object>::New(new_external<OSMRelationWrap>());
            the_Area      = v8::Persistent<v8::Object>::New(new_external<OSMAreaWrap>());
            the_Changeset = v8::Persistent<v8::Object>::New(new_external<OSMChangesetWrap>());
        }
    }

} // namespace node_osmium

NODE_MODULE(osmium, node_osmium::start)

