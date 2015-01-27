
// osmium
#include <osmium/osm/box.hpp>

#include "node_osmium.hpp"
#include "utils.hpp"

namespace node_osmium {

    extern v8::Persistent<v8::Object> module;

    v8::Handle<v8::Value> create_js_box(const osmium::Box& box) {
        v8::HandleScope scope;

        if (!box.valid()) {
            return scope.Close(v8::Undefined());
        }

        auto cf = module->Get(symbol_Coordinates);
        assert(cf->IsFunction());
        auto bf = module->Get(symbol_Box);
        assert(bf->IsFunction());

        v8::Local<v8::Value> argv_bl[2] = { v8::Number::New(box.bottom_left().lon()), v8::Number::New(box.bottom_left().lat()) };
        auto bottom_left = v8::Local<v8::Function>::Cast(cf)->NewInstance(2, argv_bl);

        v8::Local<v8::Value> argv_tr[2] = { v8::Number::New(box.top_right().lon()), v8::Number::New(box.top_right().lat()) };
        auto top_right = v8::Local<v8::Function>::Cast(cf)->NewInstance(2, argv_tr);

        v8::Local<v8::Value> argv_box[2] = { bottom_left, top_right };
        return scope.Close(v8::Local<v8::Function>::Cast(bf)->NewInstance(2, argv_box));
    }

    osmium::osm_entity_bits::type object_to_entity_bits(v8::Local<v8::Object> options) {
        osmium::osm_entity_bits::type entities = osmium::osm_entity_bits::nothing;

        v8::Local<v8::Value> want_nodes = options->Get(symbol_node);
        if (want_nodes->IsBoolean() && want_nodes->BooleanValue()) {
            entities |= osmium::osm_entity_bits::node;
        }

        v8::Local<v8::Value> want_ways = options->Get(symbol_way);
        if (want_ways->IsBoolean() && want_ways->BooleanValue()) {
            entities |= osmium::osm_entity_bits::way;
        }

        v8::Local<v8::Value> want_relations = options->Get(symbol_relation);
        if (want_relations->IsBoolean() && want_relations->BooleanValue()) {
            entities |= osmium::osm_entity_bits::relation;
        }

        v8::Local<v8::Value> want_changesets = options->Get(symbol_changeset);
        if (want_changesets->IsBoolean() && want_changesets->BooleanValue()) {
            entities |= osmium::osm_entity_bits::changeset;
        }

        return entities;
    }

} // namespace node_osmium
