
// node-osmium
#include "node_osmium.hpp"
#include "location_handler_wrap.hpp"
#include "utils.hpp"

namespace node_osmium {

    Nan::Persistent<v8::FunctionTemplate> LocationHandlerWrap::constructor;

    void LocationHandlerWrap::Initialize(v8::Local<v8::Object> target) {
        Nan::HandleScope scope;
        v8::Local<v8::FunctionTemplate> lcons = Nan::New<v8::FunctionTemplate>(LocationHandlerWrap::New);
        lcons->InstanceTemplate()->SetInternalFieldCount(1);
        lcons->SetClassName(Nan::New(symbol_LocationHandler));
        Nan::SetPrototypeMethod(lcons, "clear", clear);
        Nan::SetPrototypeMethod(lcons, "ignoreErrors", ignoreErrors);
        Nan::SetPrototypeMethod(lcons, "end", stream_end);
        target->Set(Nan::New(symbol_LocationHandler), lcons->GetFunction());
        constructor.Reset(lcons);
    }

    NAN_METHOD(LocationHandlerWrap::New) {
        if (!info.IsConstructCall()) {
            Nan::ThrowError(Nan::New("Cannot call constructor as function, you need to use 'new' keyword").ToLocalChecked());
            return;
        }

        try {
            LocationHandlerWrap* location_handler_wrap;
            if (info.Length() == 0) {
                location_handler_wrap = new LocationHandlerWrap("sparse_mem_array");
            } else {
                if (info.Length() != 1) {
                    Nan::ThrowTypeError(Nan::New("please provide a node cache type as string when creating a LocationHandler").ToLocalChecked());
                    return;
                }
                if (!info[0]->IsString()) {
                    Nan::ThrowTypeError(Nan::New("please provide a node cache type as string when creating a LocationHandler").ToLocalChecked());
                    return;
                }
                v8::String::Utf8Value index_map_type { info[0] };
                location_handler_wrap = new LocationHandlerWrap(*index_map_type);
            }
            location_handler_wrap->Wrap(info.This());
            info.GetReturnValue().Set(info.This());
            return;
        } catch (const std::exception& ex) {
            Nan::ThrowTypeError(Nan::New(ex.what()).ToLocalChecked());
            return;
        }
    }

    NAN_METHOD(LocationHandlerWrap::ignoreErrors) {
        INSTANCE_CHECK(LocationHandlerWrap, "location_handler", "ignoreErrors");
        unwrap<LocationHandlerWrap>(info.This()).ignore_errors();
        info.GetReturnValue().Set(Nan::Undefined());
        return;
    }

    NAN_METHOD(LocationHandlerWrap::clear) {
        INSTANCE_CHECK(LocationHandlerWrap, "clear", "clear");
        unwrap<LocationHandlerWrap>(info.This()).clear();
        info.GetReturnValue().Set(Nan::Undefined());
        return;
    }

    NAN_METHOD(LocationHandlerWrap::stream_end) {
        info.GetReturnValue().Set(Nan::Undefined());
        return;
    }

} // namespace node_osmium

