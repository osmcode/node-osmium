#ifndef NODE_OSMIUM
#define NODE_OSMIUM

#include "include_nan.hpp"

namespace node_osmium {

    extern Nan::Persistent<v8::String> symbol_OSMEntity;
    extern Nan::Persistent<v8::String> symbol_OSMObject;

    extern Nan::Persistent<v8::String> symbol_Node;
    extern Nan::Persistent<v8::String> symbol_node;

    extern Nan::Persistent<v8::String> symbol_Way;
    extern Nan::Persistent<v8::String> symbol_way;

    extern Nan::Persistent<v8::String> symbol_Relation;
    extern Nan::Persistent<v8::String> symbol_relation;
    extern Nan::Persistent<v8::String> symbol_type;
    extern Nan::Persistent<v8::String> symbol_ref;
    extern Nan::Persistent<v8::String> symbol_role;

    extern Nan::Persistent<v8::String> symbol_Area;
    extern Nan::Persistent<v8::String> symbol_area;

    extern Nan::Persistent<v8::String> symbol_Changeset;
    extern Nan::Persistent<v8::String> symbol_changeset;

    extern Nan::Persistent<v8::String> symbol_Coordinates;
    extern Nan::Persistent<v8::String> symbol_Box;

    extern Nan::Persistent<v8::String> symbol_generator;
    extern Nan::Persistent<v8::String> symbol_bounds;

    extern Nan::Persistent<v8::String> symbol_Buffer;
    extern Nan::Persistent<v8::String> symbol_File;
    extern Nan::Persistent<v8::String> symbol_Handler;
    extern Nan::Persistent<v8::String> symbol_LocationHandler;
    extern Nan::Persistent<v8::String> symbol_MultipolygonCollector;
    extern Nan::Persistent<v8::String> symbol_MultipolygonHandler;
    extern Nan::Persistent<v8::String> symbol_BasicReader;
    extern Nan::Persistent<v8::String> symbol_FlexReader;

    extern Nan::Persistent<v8::Object> the_Node;
    extern Nan::Persistent<v8::Object> the_Way;
    extern Nan::Persistent<v8::Object> the_Relation;
    extern Nan::Persistent<v8::Object> the_Area;
    extern Nan::Persistent<v8::Object> the_Changeset;

} // namespace node_osmium

#endif // NODE_OSMIUM
