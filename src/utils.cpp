
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

        auto cf = Nan::Get(Nan::New(module), Nan::New(symbol_Coordinates)).ToLocalChecked();
        assert(cf->IsFunction());
        auto bf = Nan::Get(Nan::New(module), Nan::New(symbol_Box)).ToLocalChecked();
        assert(bf->IsFunction());

        v8::Local<v8::Value> argv_bl[2] = { Nan::New(box.bottom_left().lon()), Nan::New<v8::Number>(box.bottom_left().lat()) };

        Nan::MaybeLocal<v8::Object> maybe_local = Nan::NewInstance(v8::Local<v8::Function>::Cast(cf),2,argv_bl);
        if (maybe_local.IsEmpty()) Nan::ThrowError("Could not create new Box instance");
        auto bottom_left = maybe_local.ToLocalChecked()->ToObject(Nan::GetCurrentContext()).ToLocalChecked();

        v8::Local<v8::Value> argv_tr[2] = { Nan::New(box.top_right().lon()), Nan::New<v8::Number>(box.top_right().lat()) };
        Nan::MaybeLocal<v8::Object> maybe_local2 = Nan::NewInstance(v8::Local<v8::Function>::Cast(cf),2,argv_tr);
        if (maybe_local2.IsEmpty()) Nan::ThrowError("Could not create new Box instance");
        auto top_right = maybe_local2.ToLocalChecked()->ToObject(Nan::GetCurrentContext()).ToLocalChecked();

        v8::Local<v8::Value> argv_box[2] = { bottom_left, top_right };

        Nan::MaybeLocal<v8::Object> maybe_local3 = Nan::NewInstance(v8::Local<v8::Function>::Cast(bf),2,argv_box);
        if (maybe_local3.IsEmpty()) Nan::ThrowError("Could not create new Box instance");

        return scope.Escape(maybe_local3.ToLocalChecked()->ToObject(Nan::GetCurrentContext()).ToLocalChecked());
    }

    osmium::osm_entity_bits::type object_to_entity_bits(v8::Local<v8::Object> options) {
        osmium::osm_entity_bits::type entities = osmium::osm_entity_bits::nothing;

        v8::Local<v8::Value> want_nodes = Nan::Get(options, Nan::New(symbol_node)).ToLocalChecked();
        if (want_nodes->IsBoolean() && Nan::To<bool>(want_nodes).ToChecked()) {
            entities |= osmium::osm_entity_bits::node;
        }

        v8::Local<v8::Value> want_ways = Nan::Get(options, Nan::New(symbol_way)).ToLocalChecked();
        if (want_ways->IsBoolean() && Nan::To<bool>(want_ways).ToChecked()) {
            entities |= osmium::osm_entity_bits::way;
        }

        v8::Local<v8::Value> want_relations = Nan::Get(options, Nan::New(symbol_relation)).ToLocalChecked();
        if (want_relations->IsBoolean() && Nan::To<bool>(want_relations).ToChecked()) {
            entities |= osmium::osm_entity_bits::relation;
        }

        v8::Local<v8::Value> want_areas = Nan::Get(options, Nan::New(symbol_area)).ToLocalChecked();
        if (want_areas->IsBoolean() && Nan::To<bool>(want_areas).ToChecked()) {
            entities |= osmium::osm_entity_bits::area;
        }

        v8::Local<v8::Value> want_changesets = Nan::Get(options, Nan::New(symbol_changeset)).ToLocalChecked();
        if (want_changesets->IsBoolean() && Nan::To<bool>(want_changesets).ToChecked()) {
            entities |= osmium::osm_entity_bits::changeset;
        }

        return entities;
    }

} // namespace node_osmium
