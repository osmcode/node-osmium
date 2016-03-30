
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
        auto attributes = static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete);
        set_accessor(constructor, "id", get_id, attributes);
        set_accessor(constructor, "version", get_version, attributes);
        set_accessor(constructor, "changeset", get_changeset, attributes);
        set_accessor(constructor, "visible", get_visible, attributes);
        set_accessor(constructor, "timestamp_seconds_since_epoch", get_timestamp, attributes);
        set_accessor(constructor, "uid", get_uid, attributes);
        set_accessor(constructor, "user", get_user, attributes);
        Nan::SetPrototypeMethod(lcons, "tags", tags);
        target->Set(Nan::New(symbol_OSMObject), lcons->GetFunction());
    }

    NAN_METHOD(OSMWrappedObject::New) {
        if (info.Length() == 1 && info[0]->IsExternal()) {
            v8::Local<v8::External> ext = v8::Local<v8::External>::Cast(info[0]);
            static_cast<OSMWrappedObject*>(ext->Value())->Wrap(info.This());
            info.GetReturnValue().Set(info.This());
            return;
        } else {
            ThrowException(v8::Exception::TypeError(Nan::New("osmium.OSMObject cannot be created in Javascript").ToLocalChecked()));
            return;
        }
    }

    NAN_METHOD(OSMWrappedObject::tags) {
        INSTANCE_CHECK(OSMWrappedObject, "Object", "tags");
        return OSMEntityWrap::tags_impl<osmium::OSMObject>(info);
    }

    NAN_METHOD(OSMWrappedObject::get_id) {
        Nan::HandleScope scope;
        info.GetReturnValue().Set(Nan::New(wrapped(info.This()).id()));
        return;
    }

    NAN_METHOD(OSMWrappedObject::get_version) {
        Nan::HandleScope scope;
        info.GetReturnValue().Set(Nan::New(wrapped(info.This()).version()));
        return;
    }

    NAN_METHOD(OSMWrappedObject::get_changeset) {
        Nan::HandleScope scope;
        info.GetReturnValue().Set(Nan::New(wrapped(info.This()).changeset()));
        return;
    }

    NAN_METHOD(OSMWrappedObject::get_visible) {
        Nan::HandleScope scope;
        info.GetReturnValue().Set(Nan::New(wrapped(info.This()).visible()));
        return;
    }

    NAN_METHOD(OSMWrappedObject::get_timestamp) {
        Nan::HandleScope scope;
        info.GetReturnValue().Set(Nan::New(uint32_t(wrapped(info.This()).timestamp())));
        return;
    }

    NAN_METHOD(OSMWrappedObject::get_uid) {
        Nan::HandleScope scope;
        info.GetReturnValue().Set(Nan::New(wrapped(info.This()).uid()));
        return;
    }

    NAN_METHOD(OSMWrappedObject::get_user) {
        Nan::HandleScope scope;
        info.GetReturnValue().Set(Nan::New(wrapped(info.This()).user()));
        return;
    }

} // namespace node_osmium
