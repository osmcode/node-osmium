
// node-osmium
#include "node_osmium.hpp"
#include "osm_entity_wrap.hpp"

namespace node_osmium {

    Nan::Persistent<v8::FunctionTemplate> OSMEntityWrap::constructor;

    void OSMEntityWrap::Initialize(v8::Local<v8::Object> target) {
        Nan::HandleScope scope;
        v8::Local<v8::FunctionTemplate> lcons = Nan::New<v8::FunctionTemplate>(OSMEntityWrap::New);
        lcons->InstanceTemplate()->SetInternalFieldCount(1);
        lcons->SetClassName(Nan::New(symbol_OSMEntity));
        target->Set(Nan::New(symbol_OSMEntity), lcons->GetFunction());
        constructor.Reset(lcons);
    }

    NAN_METHOD(OSMEntityWrap::New) {
        if (info.Length() == 1 && info[0]->IsExternal()) {
            v8::Local<v8::External> ext = v8::Local<v8::External>::Cast(info[0]);
            static_cast<OSMEntityWrap*>(ext->Value())->Wrap(info.This());
            info.GetReturnValue().Set(info.This());
            return;
        } else {
            ThrowException(v8::Exception::TypeError(Nan::New("osmium.OSMEntity cannot be created in Javascript").ToLocalChecked()));
            return;
        }
    }

} // namespace node_osmium
