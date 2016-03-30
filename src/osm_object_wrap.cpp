
// node-osmium
#include "node_osmium.hpp"
#include "osm_object_wrap.hpp"

namespace node_osmium {

    Nan::Persistent<v8::FunctionTemplate> OSMWrappedObject::constructor;

    void OSMWrappedObject::Initialize(v8::Local<v8::Object> target) {
        Nan::HandleScope scope;
        constructor = Nan::Persistent<v8::FunctionTemplate>::New(Nan::New(OSMWrappedObject::New));
        constructor->InstanceTemplate()->SetInternalFieldCount(1);
        constructor->SetClassName(symbol_OSMObject);
        auto attributes = static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete);
        set_accessor(constructor, "id", get_id, attributes);
        set_accessor(constructor, "version", get_version, attributes);
        set_accessor(constructor, "changeset", get_changeset, attributes);
        set_accessor(constructor, "visible", get_visible, attributes);
        set_accessor(constructor, "timestamp_seconds_since_epoch", get_timestamp, attributes);
        set_accessor(constructor, "uid", get_uid, attributes);
        set_accessor(constructor, "user", get_user, attributes);
        Nan::SetPrototypeMethod(constructor, "tags", tags);
        target->Set(symbol_OSMObject, constructor->GetFunction());
    }

    v8::Local<v8::Value> OSMWrappedObject::New(const v8::Arguments& info) {
        if (info.Length() == 1 && info[0]->IsExternal()) {
            v8::Local<v8::External> ext = v8::Local<v8::External>::Cast(info[0]);
            static_cast<OSMWrappedObject*>(ext->Value())->Wrap(info.This());
            return info.This();
        } else {
            return ThrowException(v8::Exception::TypeError(Nan::New("osmium.OSMObject cannot be created in Javascript").ToLocalChecked()));
        }
    }

    v8::Local<v8::Value> OSMWrappedObject::tags(const v8::Arguments& info) {
        INSTANCE_CHECK(OSMWrappedObject, "Object", "tags");
        return OSMEntityWrap::tags_impl<osmium::OSMObject>(info);
    }

    v8::Local<v8::Value> OSMWrappedObject::get_id(v8::Local<v8::String> /* property */, const v8::AccessorInfo& info) {
        Nan::HandleScope scope;
        info.GetReturnValue().Set(Nan::New(wrapped(info.This()).id()));
        return;
    }

    v8::Local<v8::Value> OSMWrappedObject::get_version(v8::Local<v8::String> /* property */, const v8::AccessorInfo& info) {
        Nan::HandleScope scope;
        info.GetReturnValue().Set(Nan::New(wrapped(info.This()).version()));
        return;
    }

    v8::Local<v8::Value> OSMWrappedObject::get_changeset(v8::Local<v8::String> /* property */, const v8::AccessorInfo& info) {
        Nan::HandleScope scope;
        info.GetReturnValue().Set(Nan::New(wrapped(info.This()).changeset()));
        return;
    }

    v8::Local<v8::Value> OSMWrappedObject::get_visible(v8::Local<v8::String> /* property */, const v8::AccessorInfo& info) {
        Nan::HandleScope scope;
        info.GetReturnValue().Set(Nan::New(wrapped(info.This()).visible()));
        return;
    }

    v8::Local<v8::Value> OSMWrappedObject::get_timestamp(v8::Local<v8::String> /* property */, const v8::AccessorInfo& info) {
        Nan::HandleScope scope;
        info.GetReturnValue().Set(Nan::New(uint32_t(wrapped(info.This()).timestamp())));
        return;
    }

    v8::Local<v8::Value> OSMWrappedObject::get_uid(v8::Local<v8::String> /* property */, const v8::AccessorInfo& info) {
        Nan::HandleScope scope;
        info.GetReturnValue().Set(Nan::New(wrapped(info.This()).uid()));
        return;
    }

    v8::Local<v8::Value> OSMWrappedObject::get_user(v8::Local<v8::String> /* property */, const v8::AccessorInfo& info) {
        Nan::HandleScope scope;
        info.GetReturnValue().Set(Nan::New(wrapped(info.This()).user()));
        return;
    }

} // namespace node_osmium
