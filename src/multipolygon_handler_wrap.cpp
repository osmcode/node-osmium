
// node-osmium
#include "node_osmium.hpp"
#include "multipolygon_handler_wrap.hpp"
#include "utils.hpp"

namespace node_osmium {

    v8::Persistent<v8::FunctionTemplate> MultipolygonHandlerWrap::constructor;

    void MultipolygonHandlerWrap::Initialize(v8::Handle<v8::Object> target) {
        v8::HandleScope scope;
        constructor = v8::Persistent<v8::FunctionTemplate>::New(v8::FunctionTemplate::New(MultipolygonHandlerWrap::New));
        constructor->InstanceTemplate()->SetInternalFieldCount(1);
        constructor->SetClassName(symbol_MultipolygonHandler);
        node::SetPrototypeMethod(constructor, "end", stream_end);
        target->Set(symbol_MultipolygonHandler, constructor->GetFunction());
    }

    v8::Handle<v8::Value> MultipolygonHandlerWrap::New(const v8::Arguments& args) {
        if (args.Length() == 1 && args[0]->IsExternal()) {
            v8::Local<v8::External> ext = v8::Local<v8::External>::Cast(args[0]);
            static_cast<MultipolygonHandlerWrap*>(ext->Value())->Wrap(args.This());
            return args.This();
        } else {
            return ThrowException(v8::Exception::TypeError(v8::String::New("osmium.MultipolygonHandler cannot be created in Javascript")));
        }
    }

    v8::Handle<v8::Value> MultipolygonHandlerWrap::stream_end(const v8::Arguments& args) {
        INSTANCE_CHECK(MultipolygonHandlerWrap, "MultipolygonHandler", "end");
        v8::HandleScope scope;
        if (args.Length() != 0) {
            return ThrowException(v8::Exception::TypeError(v8::String::New("end() doesn't take any parameters")));
        }

        auto& handler = unwrap<MultipolygonHandlerWrap>(args.This());
        handler.flush();

        return scope.Close(v8::Undefined());
    }

} // namespace node_osmium

