
// osmium
#include <osmium/osm/box.hpp>

#include "node_osmium.hpp"
#include "utils.hpp"

namespace node_osmium {

    extern Nan::Persistent<v8::Object> module;

    v8::Local<v8::Value> create_js_box(const osmium::Box& box) {
        Nan::EscapableHandleScope scope;

        if (!box.valid()) {
            return scope.Escape(Nan::Undefined());
        }

        auto cf = Nan::New(module)->Get(Nan::New(symbol_Coordinates));
        assert(cf->IsFunction());
        auto bf = Nan::New(module)->Get(Nan::New(symbol_Box));
        assert(bf->IsFunction());

        v8::Local<v8::Value> argv_bl[2] = { Nan::New(box.bottom_left().lon()), Nan::New<v8::Number>(box.bottom_left().lat()) };
        auto bottom_left = v8::Local<v8::Function>::Cast(cf)->NewInstance(2, argv_bl);

        v8::Local<v8::Value> argv_tr[2] = { Nan::New(box.top_right().lon()), Nan::New<v8::Number>(box.top_right().lat()) };
        auto top_right = v8::Local<v8::Function>::Cast(cf)->NewInstance(2, argv_tr);

        v8::Local<v8::Value> argv_box[2] = { bottom_left, top_right };
        return scope.Escape(v8::Local<v8::Function>::Cast(bf)->NewInstance(2, argv_box));
    }

    osmium::osm_entity_bits::type object_to_entity_bits(v8::Local<v8::Object> options) {
        osmium::osm_entity_bits::type entities = osmium::osm_entity_bits::nothing;

        v8::Local<v8::Value> want_nodes = options->Get(Nan::New(symbol_node));
        if (want_nodes->IsBoolean() && want_nodes->BooleanValue()) {
            entities |= osmium::osm_entity_bits::node;
        }

        v8::Local<v8::Value> want_ways = options->Get(Nan::New(symbol_way));
        if (want_ways->IsBoolean() && want_ways->BooleanValue()) {
            entities |= osmium::osm_entity_bits::way;
        }

        v8::Local<v8::Value> want_relations = options->Get(Nan::New(symbol_relation));
        if (want_relations->IsBoolean() && want_relations->BooleanValue()) {
            entities |= osmium::osm_entity_bits::relation;
        }

        v8::Local<v8::Value> want_areas = options->Get(Nan::New(symbol_area));
        if (want_areas->IsBoolean() && want_areas->BooleanValue()) {
            entities |= osmium::osm_entity_bits::area;
        }

        v8::Local<v8::Value> want_changesets = options->Get(Nan::New(symbol_changeset));
        if (want_changesets->IsBoolean() && want_changesets->BooleanValue()) {
            entities |= osmium::osm_entity_bits::changeset;
        }

        return entities;
    }

} // namespace node_osmium
