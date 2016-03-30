
// osmium
#include <osmium/geom/wkb.hpp>
#include <osmium/geom/wkt.hpp>
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

    Nan::Persistent<v8::Object> module;
    osmium::geom::WKBFactory<> wkb_factory;
    osmium::geom::WKTFactory<> wkt_factory;

    Nan::Persistent<v8::String> symbol_OSMEntity;
    Nan::Persistent<v8::String> symbol_OSMObject;

    Nan::Persistent<v8::String> symbol_Node;
    Nan::Persistent<v8::String> symbol_node;

    Nan::Persistent<v8::String> symbol_Way;
    Nan::Persistent<v8::String> symbol_way;

    Nan::Persistent<v8::String> symbol_Relation;
    Nan::Persistent<v8::String> symbol_relation;
    Nan::Persistent<v8::String> symbol_type;
    Nan::Persistent<v8::String> symbol_ref;
    Nan::Persistent<v8::String> symbol_role;

    Nan::Persistent<v8::String> symbol_Area;
    Nan::Persistent<v8::String> symbol_area;

    Nan::Persistent<v8::String> symbol_Changeset;
    Nan::Persistent<v8::String> symbol_changeset;

    Nan::Persistent<v8::String> symbol_Coordinates;
    Nan::Persistent<v8::String> symbol_Box;

    Nan::Persistent<v8::String> symbol_generator;
    Nan::Persistent<v8::String> symbol_bounds;

    Nan::Persistent<v8::String> symbol_Buffer;
    Nan::Persistent<v8::String> symbol_File;
    Nan::Persistent<v8::String> symbol_Handler;
    Nan::Persistent<v8::String> symbol_LocationHandler;
    Nan::Persistent<v8::String> symbol_MultipolygonCollector;
    Nan::Persistent<v8::String> symbol_MultipolygonHandler;
    Nan::Persistent<v8::String> symbol_BasicReader;
    Nan::Persistent<v8::String> symbol_FlexReader;

    Nan::Persistent<v8::Object> the_Node;
    Nan::Persistent<v8::Object> the_Way;
    Nan::Persistent<v8::Object> the_Relation;
    Nan::Persistent<v8::Object> the_Area;
    Nan::Persistent<v8::Object> the_Changeset;

    extern "C" {
        static void start(v8::Local<v8::Object> target) {
            Nan::HandleScope scope;
            module.Reset(target.As<v8::Object>());

            Nan::SetMethod(target, "apply_", node_osmium::apply);
            Nan::SetMethod(target, "register_filter", node_osmium::Filter::register_filter);
            
            symbol_OSMEntity.Reset(Nan::New("OSMEntity").ToLocalChecked());
            symbol_OSMObject.Reset(Nan::New("OSMObject").ToLocalChecked());
            symbol_Node.Reset(Nan::New("Node").ToLocalChecked());
            symbol_node.Reset(Nan::New("node").ToLocalChecked());
            symbol_Way.Reset(Nan::New("Way").ToLocalChecked());
            symbol_way.Reset(Nan::New("way").ToLocalChecked());
            symbol_Relation.Reset(Nan::New("Relation").ToLocalChecked());
            symbol_relation.Reset(Nan::New("relation").ToLocalChecked());
            symbol_type.Reset(Nan::New("type").ToLocalChecked());
            symbol_ref.Reset(Nan::New("ref").ToLocalChecked());
            symbol_role.Reset(Nan::New("role").ToLocalChecked());
            symbol_Area.Reset(Nan::New("Area").ToLocalChecked());
            symbol_area.Reset(Nan::New("area").ToLocalChecked());
            symbol_Changeset.Reset(Nan::New("Changeset").ToLocalChecked());
            symbol_changeset.Reset(Nan::New("changeset").ToLocalChecked());
            symbol_Coordinates.Reset(Nan::New("Coordinates").ToLocalChecked());
            symbol_Box.Reset(Nan::New("Box").ToLocalChecked());
            symbol_generator.Reset(Nan::New("generator").ToLocalChecked());
            symbol_bounds.Reset(Nan::New("bounds").ToLocalChecked());
            symbol_Buffer.Reset(Nan::New("Buffer").ToLocalChecked());
            symbol_File.Reset(Nan::New("File").ToLocalChecked());
            symbol_Handler.Reset(Nan::New("Handler").ToLocalChecked());
            symbol_LocationHandler.Reset(Nan::New("LocationHandler").ToLocalChecked());
            symbol_MultipolygonCollector.Reset(Nan::New("MultipolygonCollector").ToLocalChecked());
            symbol_MultipolygonHandler.Reset(Nan::New("MultipolygonHandler").ToLocalChecked());
            symbol_BasicReader.Reset(Nan::New("BasicReader").ToLocalChecked());
            symbol_FlexReader.Reset(Nan::New("FlexReader").ToLocalChecked());

            node_osmium::OSMEntityWrap::Initialize(target);
            node_osmium::OSMWrappedObject::Initialize(target);
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

            the_Node.Reset(new_external<OSMNodeWrap>());
            the_Way.Reset(new_external<OSMWayWrap>());
            the_Relation.Reset(new_external<OSMRelationWrap>());
            the_Area.Reset(new_external<OSMAreaWrap>());
            the_Changeset.Reset(new_external<OSMChangesetWrap>());
        }
    }

} // namespace node_osmium

NODE_MODULE(osmium, node_osmium::start)

