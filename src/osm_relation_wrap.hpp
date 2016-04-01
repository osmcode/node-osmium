#ifndef OSM_RELATION_WRAP_HPP
#define OSM_RELATION_WRAP_HPP

#include "include_nan.hpp"

// osmium
#include <osmium/osm/relation.hpp>
namespace osmium {
    class OSMEntity;
}

// node-osmium
#include "node_osmium.hpp"
#include "osm_entity_wrap.hpp"
#include "osm_object_wrap.hpp"
#include "utils.hpp"

namespace node_osmium {

    class OSMRelationWrap : public OSMWrappedObject {

        static NAN_GETTER(get_type) {
            info.GetReturnValue().Set(Nan::New(symbol_relation));
        }

        static NAN_GETTER(get_members_count);
        static NAN_METHOD(members);

    public:

        static Nan::Persistent<v8::FunctionTemplate> constructor;
        static void Initialize(v8::Local<v8::Object> target);
        static NAN_METHOD(New);

        static const osmium::Relation& wrapped(const v8::Local<v8::Object>& object) {
            return static_cast<const osmium::Relation&>(unwrap<OSMEntityWrap>(object));
        }

        OSMRelationWrap() :
            OSMWrappedObject() {
        }

        OSMRelationWrap(const osmium::OSMEntity& entity) :
            OSMWrappedObject(entity) {
        }

    private:

        ~OSMRelationWrap() = default;

    }; // class OSMRelationWrap

} // namespace node_osmium

#endif // OSM_RELATION_WRAP_HPP
