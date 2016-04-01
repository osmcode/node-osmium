#ifndef OSM_CHANGESET_WRAP_HPP
#define OSM_CHANGESET_WRAP_HPP

#include "include_nan.hpp"

// osmium
#include <osmium/osm/changeset.hpp>
namespace osmium {
    class OSMEntity;
}

// node-osmium
#include "node_osmium.hpp"
#include "osm_entity_wrap.hpp"
#include "utils.hpp"

namespace node_osmium {

    class OSMChangesetWrap : public OSMEntityWrap {

        static NAN_GETTER(get_type) {
            info.GetReturnValue().Set(Nan::New(symbol_changeset));
        }

        static NAN_GETTER(get_id);
        static NAN_GETTER(get_uid);
        static NAN_GETTER(get_user);
        static NAN_GETTER(get_num_changes);
        static NAN_GETTER(get_created_at);
        static NAN_GETTER(get_closed_at);
        static NAN_GETTER(get_open);
        static NAN_GETTER(get_closed);
        static NAN_GETTER(get_bounds);
        static NAN_METHOD(tags);

    public:

        static Nan::Persistent<v8::FunctionTemplate> constructor;
        static void Initialize(v8::Local<v8::Object> target);
        static NAN_METHOD(New);

        static const osmium::Changeset& wrapped(const v8::Local<v8::Object>& object) {
            return static_cast<const osmium::Changeset&>(unwrap<OSMEntityWrap>(object));
        }

        OSMChangesetWrap() :
            OSMEntityWrap() {
        }

        OSMChangesetWrap(const osmium::OSMEntity& entity) :
            OSMEntityWrap(entity) {
        }

    private:

        ~OSMChangesetWrap() = default;

    }; // class OSMChangesetWrap

} // namespace node_osmium

#endif // OSM_CHANGESET_WRAP_HPP
