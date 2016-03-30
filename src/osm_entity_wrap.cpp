
// node-osmium
#include "node_osmium.hpp"
#include "osm_entity_wrap.hpp"

namespace node_osmium {

    Nan::Persistent<v8::FunctionTemplate> OSMEntityWrap::constructor;

    void OSMEntityWrap::Initialize(v8::Local<v8::Object> target) {
        Nan::HandleScope scope;
        constructor = Nan::Persistent<v8::FunctionTemplate>::New(Nan::New(OSMEntityWrap::New));
        constructor->InstanceTemplate()->SetInternalFieldCount(1);
        constructor->SetClassName(symbol_OSMEntity);
        target->Set(symbol_OSMEntity, constructor->GetFunction());
    }

    v8::Local<v8::Value> OSMEntityWrap::New(const v8::Arguments& info) {
        if (info.Length() == 1 && info[0]->IsExternal()) {
            v8::Local<v8::External> ext = v8::Local<v8::External>::Cast(info[0]);
            static_cast<OSMEntityWrap*>(ext->Value())->Wrap(info.This());
            return info.This();
        } else {
            ThrowException(v8::Exception::TypeError(Nan::New("osmium.OSMEntity cannot be created in Javascript").ToLocalChecked()));
            return;
        }
    }

} // namespace node_osmium
