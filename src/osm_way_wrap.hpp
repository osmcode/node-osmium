#ifndef OSM_WAY_WRAP_HPP
#define OSM_WAY_WRAP_HPP

#include "include_nan.hpp"

// osmium
#include <osmium/osm/way.hpp>
namespace osmium {
    class OSMEntity;
}

// node-osmium
#include "node_osmium.hpp"
#include "osm_entity_wrap.hpp"
#include "osm_object_wrap.hpp"
#include "utils.hpp"

namespace node_osmium {

    class OSMWayWrap : public OSMWrappedObject {

        static NAN_GETTER(get_type) {
            info.GetReturnValue().Set(Nan::New(symbol_way));
        }

        static NAN_GETTER(get_nodes_count);
        static NAN_METHOD(node_refs);
        static NAN_METHOD(node_coordinates);
        static NAN_METHOD(wkb);
        static NAN_METHOD(wkt);

    public:

        static Nan::Persistent<v8::FunctionTemplate> constructor;
        static void Initialize(v8::Local<v8::Object> target);
        static NAN_METHOD(New);

        static const osmium::Way& wrapped(const v8::Local<v8::Object>& object) {
            return static_cast<const osmium::Way&>(unwrap<OSMEntityWrap>(object));
        }

        OSMWayWrap() :
            OSMWrappedObject() {
        }

        OSMWayWrap(const osmium::OSMEntity& entity) :
            OSMWrappedObject(entity) {
        }

    private:

        ~OSMWayWrap() = default;

    }; // class OSMWayWrap

} // namespace node_osmium

#endif // OSM_WAY_WRAP_HPP
