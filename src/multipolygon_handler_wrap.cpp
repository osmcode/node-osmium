
// node-osmium
#include "node_osmium.hpp"
#include "multipolygon_handler_wrap.hpp"
#include "utils.hpp"

namespace node_osmium {

    Nan::Persistent<v8::FunctionTemplate> MultipolygonHandlerWrap::constructor;

    void MultipolygonHandlerWrap::Initialize(v8::Local<v8::Object> target) {
        Nan::HandleScope scope;
        constructor = Nan::Persistent<v8::FunctionTemplate>::New(Nan::New(MultipolygonHandlerWrap::New));
        constructor->InstanceTemplate()->SetInternalFieldCount(1);
        constructor->SetClassName(symbol_MultipolygonHandler);
        Nan::SetPrototypeMethod(constructor, "end", stream_end);
        target->Set(symbol_MultipolygonHandler, constructor->GetFunction());
    }

    v8::Local<v8::Value> MultipolygonHandlerWrap::New(const v8::Arguments& info) {
        if (info.Length() == 1 && info[0]->IsExternal()) {
            v8::Local<v8::External> ext = v8::Local<v8::External>::Cast(info[0]);
            static_cast<MultipolygonHandlerWrap*>(ext->Value())->Wrap(info.This());
            return info.This();
        } else {
            return ThrowException(v8::Exception::TypeError(Nan::New("osmium.MultipolygonHandler cannot be created in Javascript").ToLocalChecked()));
        }
    }

    v8::Local<v8::Value> MultipolygonHandlerWrap::stream_end(const v8::Arguments& info) {
        INSTANCE_CHECK(MultipolygonHandlerWrap, "MultipolygonHandler", "end");
        Nan::HandleScope scope;
        if (info.Length() != 0) {
            return ThrowException(v8::Exception::TypeError(Nan::New("end() doesn't take any parameters").ToLocalChecked()));
        }

        auto& handler = unwrap<MultipolygonHandlerWrap>(info.This());
        handler.flush();

        info.GetReturnValue().Set(Nan::Undefined());
        return;
    }

} // namespace node_osmium

