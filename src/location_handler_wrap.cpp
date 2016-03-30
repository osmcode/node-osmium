
// node-osmium
#include "node_osmium.hpp"
#include "location_handler_wrap.hpp"
#include "utils.hpp"

namespace node_osmium {

    Nan::Persistent<v8::FunctionTemplate> LocationHandlerWrap::constructor;

    void LocationHandlerWrap::Initialize(v8::Local<v8::Object> target) {
        Nan::HandleScope scope;
        constructor = Nan::Persistent<v8::FunctionTemplate>::New(Nan::New(LocationHandlerWrap::New));
        constructor->InstanceTemplate()->SetInternalFieldCount(1);
        constructor->SetClassName(symbol_LocationHandler);
        Nan::SetPrototypeMethod(constructor, "clear", clear);
        Nan::SetPrototypeMethod(constructor, "ignoreErrors", ignoreErrors);
        Nan::SetPrototypeMethod(constructor, "end", stream_end);
        target->Set(symbol_LocationHandler, constructor->GetFunction());
    }

    v8::Local<v8::Value> LocationHandlerWrap::New(const v8::Arguments& info) {
        Nan::HandleScope scope;
        if (!info.IsConstructCall()) {
            ThrowException(v8::Exception::Error(Nan::New("Cannot call constructor as function, you need to use 'new' keyword").ToLocalChecked()));
            return;
        }

        try {
            LocationHandlerWrap* location_handler_wrap;
            if (info.Length() == 0) {
                location_handler_wrap = new LocationHandlerWrap("sparse_mem_array");
            } else {
                if (info.Length() != 1) {
                    ThrowException(v8::Exception::TypeError(Nan::New("please provide a node cache type as string when creating a LocationHandler").ToLocalChecked()));
                    return;
                }
                if (!info[0]->IsString()) {
                    ThrowException(v8::Exception::TypeError(Nan::New("please provide a node cache type as string when creating a LocationHandler").ToLocalChecked()));
                    return;
                }
                v8::String::Utf8Value index_map_type { info[0] };
                location_handler_wrap = new LocationHandlerWrap(*index_map_type);
            }
            location_handler_wrap->Wrap(info.This());
            return info.This();
        } catch (const std::exception& ex) {
            ThrowException(v8::Exception::TypeError(Nan::New(ex.what())));
            return;
        }
    }

    v8::Local<v8::Value> LocationHandlerWrap::ignoreErrors(const v8::Arguments& info) {
        INSTANCE_CHECK(LocationHandlerWrap, "location_handler", "ignoreErrors");
        Nan::HandleScope scope;
        unwrap<LocationHandlerWrap>(info.This()).ignore_errors();
        info.GetReturnValue().Set(Nan::Undefined());
        return;
    }

    v8::Local<v8::Value> LocationHandlerWrap::clear(const v8::Arguments& info) {
        INSTANCE_CHECK(LocationHandlerWrap, "clear", "clear");
        Nan::HandleScope scope;
        unwrap<LocationHandlerWrap>(info.This()).clear();
        info.GetReturnValue().Set(Nan::Undefined());
        return;
    }

    v8::Local<v8::Value> LocationHandlerWrap::stream_end(const v8::Arguments& info) {
        return Nan::Undefined();
    }

} // namespace node_osmium

