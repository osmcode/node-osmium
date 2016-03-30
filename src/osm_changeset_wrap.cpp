
// node-osmium
#include "osm_changeset_wrap.hpp"
#include "utils.hpp"

namespace node_osmium {

    extern Nan::Persistent<v8::Object> module;

    Nan::Persistent<v8::FunctionTemplate> OSMChangesetWrap::constructor;

    void OSMChangesetWrap::Initialize(v8::Local<v8::Object> target) {
        Nan::HandleScope scope;

        v8::Local<v8::FunctionTemplate> lcons = Nan::New<v8::FunctionTemplate>(OSMChangesetWrap::New);
        constructor->Inherit(OSMEntityWrap::constructor);
        lcons->InstanceTemplate()->SetInternalFieldCount(1);
        lcons->SetClassName(Nan::New(symbol_Changeset));
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
        Nan::SetPrototypeMethod(lcons, "tags", tags);
        target->Set(Nan::New(symbol_Changeset), lcons->GetFunction());
    }

    NAN_METHOD(OSMChangesetWrap::New) {
        if (info.Length() == 1 && info[0]->IsExternal()) {
            v8::Local<v8::External> ext = v8::Local<v8::External>::Cast(info[0]);
            static_cast<OSMChangesetWrap*>(ext->Value())->Wrap(info.This());
            info.GetReturnValue().Set(info.This());
            return;
        } else {
            ThrowException(v8::Exception::TypeError(Nan::New("osmium.Changeset cannot be created in Javascript").ToLocalChecked()));
            return;
        }
    }

    NAN_METHOD(OSMChangesetWrap::tags) {
        INSTANCE_CHECK(OSMChangesetWrap, "Changeset", "tags");
        return OSMEntityWrap::tags_impl<osmium::Changeset>(info);
    }

    NAN_METHOD(OSMChangesetWrap::get_id) {
        Nan::HandleScope scope;
        info.GetReturnValue().Set(Nan::New(wrapped(info.This()).id()));
        return;
    }

    NAN_METHOD(OSMChangesetWrap::get_uid) {
        Nan::HandleScope scope;
        info.GetReturnValue().Set(Nan::New(wrapped(info.This()).uid()));
        return;
    }

    NAN_METHOD(OSMChangesetWrap::get_user) {
        Nan::HandleScope scope;
        info.GetReturnValue().Set(Nan::New(wrapped(info.This()).user()));
        return;
    }

    NAN_METHOD(OSMChangesetWrap::get_num_changes) {
        Nan::HandleScope scope;
        info.GetReturnValue().Set(Nan::New(wrapped(info.This()).num_changes()));
        return;
    }

    NAN_METHOD(OSMChangesetWrap::get_created_at) {
        Nan::HandleScope scope;
        info.GetReturnValue().Set(Nan::New(uint32_t(wrapped(info.This()).created_at())));
        return;
    }

    NAN_METHOD(OSMChangesetWrap::get_closed_at) {
        Nan::HandleScope scope;
        info.GetReturnValue().Set(Nan::New(uint32_t(wrapped(info.This()).closed_at())));
        return;
    }

    NAN_METHOD(OSMChangesetWrap::get_open) {
        Nan::HandleScope scope;
        info.GetReturnValue().Set(Nan::New(wrapped(info.This()).open()));
        return;
    }

    NAN_METHOD(OSMChangesetWrap::get_closed) {
        Nan::HandleScope scope;
        info.GetReturnValue().Set(Nan::New(wrapped(info.This()).closed()));
        return;
    }

    NAN_METHOD(OSMChangesetWrap::get_bounds) {
        return create_js_box(wrapped(info.This()).bounds());
    }

} // namespace node_osmium
