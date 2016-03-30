
// node-osmium
#include "osm_changeset_wrap.hpp"
#include "utils.hpp"

namespace node_osmium {

    extern Nan::Persistent<v8::Object> module;

    Nan::Persistent<v8::FunctionTemplate> OSMChangesetWrap::constructor;

    void OSMChangesetWrap::Initialize(v8::Local<v8::Object> target) {
        Nan::HandleScope scope;

        constructor = Nan::Persistent<v8::FunctionTemplate>::New(Nan::New(OSMChangesetWrap::New));
        constructor->Inherit(OSMEntityWrap::constructor);
        constructor->InstanceTemplate()->SetInternalFieldCount(1);
        constructor->SetClassName(symbol_Changeset);
        auto attributes = static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete);
        set_accessor(constructor, "type", get_type, attributes);
        set_accessor(constructor, "id", get_id, attributes);
        set_accessor(constructor, "uid", get_uid, attributes);
        set_accessor(constructor, "user", get_user, attributes);
        set_accessor(constructor, "num_changes", get_num_changes, attributes);
        set_accessor(constructor, "created_at_seconds_since_epoch", get_created_at, attributes);
        set_accessor(constructor, "closed_at_seconds_since_epoch", get_closed_at, attributes);
        set_accessor(constructor, "open", get_open, attributes);
        set_accessor(constructor, "closed", get_closed, attributes);
        set_accessor(constructor, "bounds", get_bounds, attributes);
        Nan::SetPrototypeMethod(constructor, "tags", tags);
        target->Set(symbol_Changeset, constructor->GetFunction());
    }

    v8::Local<v8::Value> OSMChangesetWrap::New(const v8::Arguments& info) {
        if (info.Length() == 1 && info[0]->IsExternal()) {
            v8::Local<v8::External> ext = v8::Local<v8::External>::Cast(info[0]);
            static_cast<OSMChangesetWrap*>(ext->Value())->Wrap(info.This());
            return info.This();
        } else {
            return ThrowException(v8::Exception::TypeError(Nan::New("osmium.Changeset cannot be created in Javascript").ToLocalChecked()));
        }
    }

    v8::Local<v8::Value> OSMChangesetWrap::tags(const v8::Arguments& info) {
        INSTANCE_CHECK(OSMChangesetWrap, "Changeset", "tags");
        return OSMEntityWrap::tags_impl<osmium::Changeset>(info);
    }

    v8::Local<v8::Value> OSMChangesetWrap::get_id(v8::Local<v8::String> /* property */, const v8::AccessorInfo& info) {
        Nan::HandleScope scope;
        info.GetReturnValue().Set(Nan::New(wrapped(info.This()).id()));
        return;
    }

    v8::Local<v8::Value> OSMChangesetWrap::get_uid(v8::Local<v8::String> /* property */, const v8::AccessorInfo& info) {
        Nan::HandleScope scope;
        info.GetReturnValue().Set(Nan::New(wrapped(info.This()).uid()));
        return;
    }

    v8::Local<v8::Value> OSMChangesetWrap::get_user(v8::Local<v8::String> /* property */, const v8::AccessorInfo& info) {
        Nan::HandleScope scope;
        info.GetReturnValue().Set(Nan::New(wrapped(info.This()).user()));
        return;
    }

    v8::Local<v8::Value> OSMChangesetWrap::get_num_changes(v8::Local<v8::String> /* property */, const v8::AccessorInfo& info) {
        Nan::HandleScope scope;
        info.GetReturnValue().Set(Nan::New(wrapped(info.This()).num_changes()));
        return;
    }

    v8::Local<v8::Value> OSMChangesetWrap::get_created_at(v8::Local<v8::String> /* property */, const v8::AccessorInfo& info) {
        Nan::HandleScope scope;
        info.GetReturnValue().Set(Nan::New(uint32_t(wrapped(info.This()).created_at())));
        return;
    }

    v8::Local<v8::Value> OSMChangesetWrap::get_closed_at(v8::Local<v8::String> /* property */, const v8::AccessorInfo& info) {
        Nan::HandleScope scope;
        info.GetReturnValue().Set(Nan::New(uint32_t(wrapped(info.This()).closed_at())));
        return;
    }

    v8::Local<v8::Value> OSMChangesetWrap::get_open(v8::Local<v8::String> /* property */, const v8::AccessorInfo& info) {
        Nan::HandleScope scope;
        info.GetReturnValue().Set(Nan::New(wrapped(info.This()).open()));
        return;
    }

    v8::Local<v8::Value> OSMChangesetWrap::get_closed(v8::Local<v8::String> /* property */, const v8::AccessorInfo& info) {
        Nan::HandleScope scope;
        info.GetReturnValue().Set(Nan::New(wrapped(info.This()).closed()));
        return;
    }

    v8::Local<v8::Value> OSMChangesetWrap::get_bounds(v8::Local<v8::String> /* property */, const v8::AccessorInfo& info) {
        return create_js_box(wrapped(info.This()).bounds());
    }

} // namespace node_osmium
