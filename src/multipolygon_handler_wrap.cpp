
// node-osmium
#include "node_osmium.hpp"
#include "multipolygon_handler_wrap.hpp"
#include "utils.hpp"

namespace node_osmium {

    Nan::Persistent<v8::FunctionTemplate> MultipolygonHandlerWrap::constructor;

    void MultipolygonHandlerWrap::Initialize(v8::Local<v8::Object> target) {
        Nan::HandleScope scope;
        v8::Local<v8::FunctionTemplate> lcons = Nan::New<v8::FunctionTemplate>(MultipolygonHandlerWrap::New);
        lcons->InstanceTemplate()->SetInternalFieldCount(1);
        lcons->SetClassName(Nan::New(symbol_MultipolygonHandler));
        Nan::SetPrototypeMethod(lcons, "end", stream_end);
        target->Set(Nan::New(symbol_MultipolygonHandler), lcons->GetFunction());
        constructor.Reset(lcons);
    }

    NAN_METHOD(MultipolygonHandlerWrap::New) {
        if (info.Length() == 1 && info[0]->IsExternal()) {
            v8::Local<v8::External> ext = v8::Local<v8::External>::Cast(info[0]);
            static_cast<MultipolygonHandlerWrap*>(ext->Value())->Wrap(info.This());
            info.GetReturnValue().Set(info.This());
            return;
        } else {
            Nan::ThrowTypeError(Nan::New("osmium.MultipolygonHandler cannot be created in Javascript").ToLocalChecked());
            return;
        }
    }

    NAN_METHOD(MultipolygonHandlerWrap::stream_end) {
        INSTANCE_CHECK(MultipolygonHandlerWrap, "MultipolygonHandler", "end");
        Nan::HandleScope scope;
        if (info.Length() != 0) {
            Nan::ThrowTypeError(Nan::New("end() doesn't take any parameters").ToLocalChecked());
            return;
        }

        auto& handler = unwrap<MultipolygonHandlerWrap>(info.This());
        handler.flush();

        info.GetReturnValue().Set(Nan::Undefined());
        return;
    }

} // namespace node_osmium

