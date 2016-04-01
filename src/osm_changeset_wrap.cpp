
// node-osmium
#include "osm_changeset_wrap.hpp"
#include "utils.hpp"

namespace node_osmium {

    extern Nan::Persistent<v8::Object> module;

    Nan::Persistent<v8::FunctionTemplate> OSMChangesetWrap::constructor;

    void OSMChangesetWrap::Initialize(v8::Local<v8::Object> target) {
        Nan::HandleScope scope;

        v8::Local<v8::FunctionTemplate> lcons = Nan::New<v8::FunctionTemplate>(OSMChangesetWrap::New);
        lcons->Inherit(Nan::New(OSMEntityWrap::constructor));
        lcons->InstanceTemplate()->SetInternalFieldCount(1);
        lcons->SetClassName(Nan::New(symbol_Changeset));
        ATTR(lcons, "type", get_type);
        ATTR(lcons, "id", get_id);
        ATTR(lcons, "uid", get_uid);
        ATTR(lcons, "user", get_user);
        ATTR(lcons, "num_changes", get_num_changes);
        ATTR(lcons, "created_at_seconds_since_epoch", get_created_at);
        ATTR(lcons, "closed_at_seconds_since_epoch", get_closed_at);
        ATTR(lcons, "open", get_open);
        ATTR(lcons, "closed", get_closed);
        ATTR(lcons, "bounds", get_bounds);
        Nan::SetPrototypeMethod(lcons, "tags", tags);
        target->Set(Nan::New(symbol_Changeset), lcons->GetFunction());
        constructor.Reset(lcons);
    }

    NAN_METHOD(OSMChangesetWrap::New) {
        if (info.Length() == 1 && info[0]->IsExternal()) {
            v8::Local<v8::External> ext = v8::Local<v8::External>::Cast(info[0]);
            static_cast<OSMChangesetWrap*>(ext->Value())->Wrap(info.This());
            info.GetReturnValue().Set(info.This());
            return;
        } else {
            Nan::ThrowTypeError(Nan::New("osmium.Changeset cannot be created in Javascript").ToLocalChecked());
            return;
        }
    }

    NAN_METHOD(OSMChangesetWrap::tags) {
        INSTANCE_CHECK(OSMChangesetWrap, "Changeset", "tags");
        return OSMEntityWrap::tags_impl<osmium::Changeset>(info);
    }

    NAN_GETTER(OSMChangesetWrap::get_id) {
        info.GetReturnValue().Set(Nan::New(wrapped(info.This()).id()));
        return;
    }

    NAN_GETTER(OSMChangesetWrap::get_uid) {
        info.GetReturnValue().Set(Nan::New(wrapped(info.This()).uid()));
        return;
    }

    NAN_GETTER(OSMChangesetWrap::get_user) {
        info.GetReturnValue().Set(Nan::New(wrapped(info.This()).user()).ToLocalChecked());
        return;
    }

    NAN_GETTER(OSMChangesetWrap::get_num_changes) {
        info.GetReturnValue().Set(Nan::New(wrapped(info.This()).num_changes()));
        return;
    }

    NAN_GETTER(OSMChangesetWrap::get_created_at) {
        info.GetReturnValue().Set(Nan::New(uint32_t(wrapped(info.This()).created_at())));
        return;
    }

    NAN_GETTER(OSMChangesetWrap::get_closed_at) {
        info.GetReturnValue().Set(Nan::New(uint32_t(wrapped(info.This()).closed_at())));
        return;
    }

    NAN_GETTER(OSMChangesetWrap::get_open) {
        info.GetReturnValue().Set(Nan::New(wrapped(info.This()).open()));
        return;
    }

    NAN_GETTER(OSMChangesetWrap::get_closed) {
        info.GetReturnValue().Set(Nan::New(wrapped(info.This()).closed()));
        return;
    }

    NAN_GETTER(OSMChangesetWrap::get_bounds) {
        info.GetReturnValue().Set(create_js_box(wrapped(info.This()).bounds()));
        return;
    }

} // namespace node_osmium
