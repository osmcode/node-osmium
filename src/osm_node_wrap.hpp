#ifndef OSM_NODE_WRAP_HPP
#define OSM_NODE_WRAP_HPP

#include "include_nan.hpp"

// osmium
#include <osmium/osm/node.hpp>
namespace osmium {
    class OSMEntity;
}

// node-osmium
#include "node_osmium.hpp"
#include "osm_entity_wrap.hpp"
#include "osm_object_wrap.hpp"
#include "utils.hpp"

namespace node_osmium {

    class OSMNodeWrap : public OSMWrappedObject {

        static NAN_GETTER(get_type) {
            info.GetReturnValue().Set(Nan::New(symbol_node));
        }

        static NAN_GETTER(get_coordinates);
        static NAN_GETTER(get_lon);
        static NAN_GETTER(get_lat);
        static NAN_METHOD(wkb);
        static NAN_METHOD(wkt);

    public:

        static Nan::Persistent<v8::FunctionTemplate> constructor;
        static void Initialize(v8::Local<v8::Object> target);
        static NAN_METHOD(New);

        static const osmium::Node& wrapped(const v8::Local<v8::Object>& object) {
            return static_cast<const osmium::Node&>(unwrap<OSMEntityWrap>(object));
        }

        OSMNodeWrap() :
            OSMWrappedObject() {
        }

        OSMNodeWrap(const osmium::OSMEntity& entity) :
            OSMWrappedObject(entity) {
        }

    private:

        ~OSMNodeWrap() = default;

    }; // class OSMNodeWrap

} // namespace node_osmium

#endif // OSM_NODE_WRAP_HPP
