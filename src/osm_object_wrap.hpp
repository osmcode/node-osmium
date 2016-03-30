#ifndef OSM_OBJECT_WRAP_HPP
#define OSM_OBJECT_WRAP_HPP

#include "include_nan.hpp"

// osmium
#include <osmium/osm/object.hpp>
namespace osmium {
    class OSMEntity;
}

// node-osmium
#include "osm_entity_wrap.hpp"
#include "utils.hpp"

namespace node_osmium {

    class OSMWrappedObject : public OSMEntityWrap {

        static NAN_GETTER(get_id);
        static NAN_GETTER(get_version);
        static NAN_GETTER(get_changeset);
        static NAN_GETTER(get_visible);
        static NAN_GETTER(get_timestamp);
        static NAN_GETTER(get_uid);
        static NAN_GETTER(get_user);
        static NAN_METHOD(tags);

    public:

        static Nan::Persistent<v8::FunctionTemplate> constructor;
        static void Initialize(v8::Handle<v8::Object> target);
        static NAN_METHOD(New);

        static const osmium::OSMObject& wrapped(const v8::Local<v8::Object>& object) {
            return static_cast<const osmium::OSMObject&>(unwrap<OSMEntityWrap>(object));
        }

        OSMWrappedObject() :
            OSMEntityWrap() {
        }

        OSMWrappedObject(const osmium::OSMEntity& entity) :
            OSMEntityWrap(entity) {
        }

    protected:

        virtual ~OSMWrappedObject() = default;

    }; // class OSMWrappedObject

} // namespace node_osmium

#endif // OSM_OBJECT_WRAP_HPP
