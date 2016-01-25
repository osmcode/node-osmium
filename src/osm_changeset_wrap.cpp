
// node-osmium
#include "osm_changeset_wrap.hpp"
#include "utils.hpp"

namespace node_osmium {

    extern v8::Persistent<v8::Object> module;

    v8::Persistent<v8::FunctionTemplate> OSMChangesetWrap::constructor;

    void OSMChangesetWrap::Initialize(v8::Handle<v8::Object> target) {
        v8::HandleScope scope;

        constructor = v8::Persistent<v8::FunctionTemplate>::New(v8::FunctionTemplate::New(OSMChangesetWrap::New));
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
        node::SetPrototypeMethod(constructor, "tags", tags);
        target->Set(symbol_Changeset, constructor->GetFunction());
    }

    v8::Handle<v8::Value> OSMChangesetWrap::New(const v8::Arguments& args) {
        if (args.Length() == 1 && args[0]->IsExternal()) {
            v8::Local<v8::External> ext = v8::Local<v8::External>::Cast(args[0]);
            static_cast<OSMChangesetWrap*>(ext->Value())->Wrap(args.This());
            return args.This();
        } else {
            return ThrowException(v8::Exception::TypeError(v8::String::New("osmium.Changeset cannot be created in Javascript")));
        }
    }

    v8::Handle<v8::Value> OSMChangesetWrap::tags(const v8::Arguments& args) {
        INSTANCE_CHECK(OSMChangesetWrap, "Changeset", "tags");
        return OSMEntityWrap::tags_impl<osmium::Changeset>(args);
    }

    v8::Handle<v8::Value> OSMChangesetWrap::get_id(v8::Local<v8::String> /* property */, const v8::AccessorInfo& info) {
        v8::HandleScope scope;
        return scope.Close(v8::Number::New(wrapped(info.This()).id()));
    }

    v8::Handle<v8::Value> OSMChangesetWrap::get_uid(v8::Local<v8::String> /* property */, const v8::AccessorInfo& info) {
        v8::HandleScope scope;
        return scope.Close(v8::Uint32::New(wrapped(info.This()).uid()));
    }

    v8::Handle<v8::Value> OSMChangesetWrap::get_user(v8::Local<v8::String> /* property */, const v8::AccessorInfo& info) {
        v8::HandleScope scope;
        return scope.Close(v8::String::New(wrapped(info.This()).user()));
    }

    v8::Handle<v8::Value> OSMChangesetWrap::get_num_changes(v8::Local<v8::String> /* property */, const v8::AccessorInfo& info) {
        v8::HandleScope scope;
        return scope.Close(v8::Uint32::New(wrapped(info.This()).num_changes()));
    }

    v8::Handle<v8::Value> OSMChangesetWrap::get_created_at(v8::Local<v8::String> /* property */, const v8::AccessorInfo& info) {
        v8::HandleScope scope;
        return scope.Close(v8::Uint32::New(uint32_t(wrapped(info.This()).created_at())));
    }

    v8::Handle<v8::Value> OSMChangesetWrap::get_closed_at(v8::Local<v8::String> /* property */, const v8::AccessorInfo& info) {
        v8::HandleScope scope;
        return scope.Close(v8::Uint32::New(uint32_t(wrapped(info.This()).closed_at())));
    }

    v8::Handle<v8::Value> OSMChangesetWrap::get_open(v8::Local<v8::String> /* property */, const v8::AccessorInfo& info) {
        v8::HandleScope scope;
        return scope.Close(v8::Boolean::New(wrapped(info.This()).open()));
    }

    v8::Handle<v8::Value> OSMChangesetWrap::get_closed(v8::Local<v8::String> /* property */, const v8::AccessorInfo& info) {
        v8::HandleScope scope;
        return scope.Close(v8::Boolean::New(wrapped(info.This()).closed()));
    }

    v8::Handle<v8::Value> OSMChangesetWrap::get_bounds(v8::Local<v8::String> /* property */, const v8::AccessorInfo& info) {
        return create_js_box(wrapped(info.This()).bounds());
    }

} // namespace node_osmium
