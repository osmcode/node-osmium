
// node-osmium
#include "node_osmium.hpp"
#include "osm_object_wrap.hpp"

namespace node_osmium {

    Nan::Persistent<v8::FunctionTemplate> OSMWrappedObject::constructor;

    void OSMWrappedObject::Initialize(v8::Local<v8::Object> target) {
        Nan::HandleScope scope;
        v8::Local<v8::FunctionTemplate> lcons = Nan::New<v8::FunctionTemplate>(OSMWrappedObject::New);
        lcons->InstanceTemplate()->SetInternalFieldCount(1);
        lcons->SetClassName(Nan::New(symbol_OSMObject));
        ATTR(lcons, "id", get_id);
        ATTR(lcons, "version", get_version);
        ATTR(lcons, "changeset", get_changeset);
        ATTR(lcons, "visible", get_visible);
        ATTR(lcons, "timestamp_seconds_since_epoch", get_timestamp);
        ATTR(lcons, "uid", get_uid);
        ATTR(lcons, "user", get_user);
        Nan::SetPrototypeMethod(lcons, "tags", tags);
        target->Set(Nan::New(symbol_OSMObject), lcons->GetFunction());
        constructor.Reset(lcons);
    }

    NAN_METHOD(OSMWrappedObject::New) {
        if (info.Length() == 1 && info[0]->IsExternal()) {
            v8::Local<v8::External> ext = v8::Local<v8::External>::Cast(info[0]);
            static_cast<OSMWrappedObject*>(ext->Value())->Wrap(info.This());
            info.GetReturnValue().Set(info.This());
            return;
        } else {
            Nan::ThrowTypeError(Nan::New("osmium.OSMObject cannot be created in Javascript").ToLocalChecked());
            return;
        }
    }

    NAN_METHOD(OSMWrappedObject::tags) {
        INSTANCE_CHECK(OSMWrappedObject, "Object", "tags");
        return OSMEntityWrap::tags_impl<osmium::OSMObject>(info);
    }

    NAN_GETTER(OSMWrappedObject::get_id) {
        Nan::HandleScope scope;
        info.GetReturnValue().Set(Nan::New<v8::Number>(wrapped(info.This()).id()));
        return;
    }

    NAN_GETTER(OSMWrappedObject::get_version) {
        Nan::HandleScope scope;
        info.GetReturnValue().Set(Nan::New(wrapped(info.This()).version()));
        return;
    }

    NAN_GETTER(OSMWrappedObject::get_changeset) {
        Nan::HandleScope scope;
        info.GetReturnValue().Set(Nan::New(wrapped(info.This()).changeset()));
        return;
    }

    NAN_GETTER(OSMWrappedObject::get_visible) {
        Nan::HandleScope scope;
        info.GetReturnValue().Set(Nan::New(wrapped(info.This()).visible()));
        return;
    }

    NAN_GETTER(OSMWrappedObject::get_timestamp) {
        Nan::HandleScope scope;
        info.GetReturnValue().Set(Nan::New(uint32_t(wrapped(info.This()).timestamp())));
        return;
    }

    NAN_GETTER(OSMWrappedObject::get_uid) {
        Nan::HandleScope scope;
        info.GetReturnValue().Set(Nan::New(wrapped(info.This()).uid()));
        return;
    }

    NAN_GETTER(OSMWrappedObject::get_user) {
        Nan::HandleScope scope;
        info.GetReturnValue().Set(Nan::New(wrapped(info.This()).user()).ToLocalChecked());
        return;
    }

} // namespace node_osmium
