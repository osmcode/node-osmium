#ifndef OSM_AREA_WRAP_HPP
#define OSM_AREA_WRAP_HPP

#include "include_nan.hpp"

// osmium
#include <osmium/osm/area.hpp>
namespace osmium {
    class OSMEntity;
}

// node-osmium
#include "node_osmium.hpp"
#include "osm_entity_wrap.hpp"
#include "osm_object_wrap.hpp"
#include "utils.hpp"

namespace node_osmium {

    class OSMAreaWrap : public OSMWrappedObject {

        static NAN_GETTER(get_type) {
            info.GetReturnValue().Set(Nan::New(symbol_area));
        }

        static NAN_GETTER(get_orig_id);
        static NAN_METHOD(from_way);

        static NAN_METHOD(wkb);
        static NAN_METHOD(wkt);
        static NAN_METHOD(coordinates);

    public:

        static Nan::Persistent<v8::FunctionTemplate> constructor;
        static void Initialize(v8::Local<v8::Object> target);
        static NAN_METHOD(New);

        static const osmium::Area& wrapped(const v8::Local<v8::Object>& object) {
            return static_cast<const osmium::Area&>(unwrap<OSMEntityWrap>(object));
        }

        OSMAreaWrap() :
            OSMWrappedObject() {
        }

        OSMAreaWrap(const osmium::OSMEntity& entity) :
            OSMWrappedObject(entity) {
        }

    private:

        ~OSMAreaWrap() = default;

    }; // class OSMAreaWrap

} // namespace node_osmium

#endif // OSM_AREA_WRAP_HPP
