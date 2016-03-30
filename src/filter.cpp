
#include "filter.hpp"

#include <cassert>
#include <limits>
#include <memory>
#include <vector>

#include <osmium/osm.hpp>
#include <osmium/tags/taglist.hpp>

namespace node_osmium {

    std::vector<std::unique_ptr<Filter>> Filter::all_filters;

    void Filter::init_filters() {
        // default filter allowing everything is always at id 0
        all_filters.emplace_back(new Filter());
    }

    v8::Local<v8::Value> Filter::register_filter(const v8::Arguments& info) {
        Nan::HandleScope scope;

        if (info.Length() == 1 && info[0]->IsObject()) {
            auto object = info[0]->ToObject();
            // XXX check that object is of class Filter
            Filter::all_filters.emplace_back(new Filter(object));
            assert(Filter::all_filters.size() < std::numeric_limits<int32_t>::max());
            info.GetReturnValue().Set(Nan::New(static_cast<int32_t>(Filter::all_filters.size() - 1)));
            return;
        }

        return ThrowException(v8::Exception::Error(Nan::New("registering filter failed").ToLocalChecked()));
    }

    const Filter& Filter::get_filter(size_t id) {
        if (id >= Filter::all_filters.size()) {
            id = 0;
        }
        assert(Filter::all_filters.size() > id);
        return *Filter::all_filters[id];
    }

    void Filter::setup_filter(v8::Array* array, osmium::item_type item_type) {
        if (array->Length() > 0) {
            m_entity_bits |= osmium::osm_entity_bits::from_item_type(item_type);
        }
        for (uint32_t i = 0; i < array->Length(); ++i) {
            auto element = array->Get(i);
            assert(element->IsArray());
            auto key_value = v8::Array::Cast(*element);
            assert(key_value->Length() == 2);
            v8::Local<v8::Value> key = key_value->Get(0);
            v8::Local<v8::Value> value = key_value->Get(1);

            if (key->IsString()) {
                v8::String::Utf8Value key_string { key };
                if (value->IsString()) {
                    v8::String::Utf8Value value_string { value };
                    m_filters[static_cast<uint16_t>(item_type) - 1].add(true, *key_string, *value_string);
                } else {
                    m_filters[static_cast<uint16_t>(item_type) - 1].add(true, *key_string);
                }
            } else if (key->IsNull()) {
                m_tagged_entity_bits |= osmium::osm_entity_bits::from_item_type(item_type);
            }
        }
    }

    Filter::Filter() :
        m_entity_bits(osmium::osm_entity_bits::all),
        m_tagged_entity_bits(osmium::osm_entity_bits::nothing) {
    }

    Filter::Filter(v8::Local<v8::Object> object) :
        m_entity_bits(osmium::osm_entity_bits::nothing),
        m_tagged_entity_bits(osmium::osm_entity_bits::nothing) {
        setup_filter(v8::Array::Cast(*object->Get(NODE_PSYMBOL("_node"))),      osmium::item_type::node);
        setup_filter(v8::Array::Cast(*object->Get(NODE_PSYMBOL("_way"))),       osmium::item_type::way);
        setup_filter(v8::Array::Cast(*object->Get(NODE_PSYMBOL("_relation"))),  osmium::item_type::relation);
        setup_filter(v8::Array::Cast(*object->Get(NODE_PSYMBOL("_changeset"))), osmium::item_type::changeset);
        setup_filter(v8::Array::Cast(*object->Get(NODE_PSYMBOL("_area"))),      osmium::item_type::area);
    }

    bool Filter::match(const osmium::OSMEntity& entity) const {
        if ((m_entity_bits & osmium::osm_entity_bits::from_item_type(entity.type())) == 0) {
            return false;
        }

        const auto& tags = (entity.type() == osmium::item_type::changeset) ? static_cast<const osmium::Changeset&>(entity).tags() : static_cast<const osmium::OSMObject&>(entity).tags();

        if ((m_tagged_entity_bits & osmium::osm_entity_bits::from_item_type(entity.type())) != 0) {
            return !tags.empty();
        }

        const auto& filter = m_filters[static_cast<uint16_t>(entity.type()) - 1];
        if (filter.empty()) {
            return true;
        }

        return osmium::tags::match_any_of(tags, filter);
    }

} // namespace node_osmium
