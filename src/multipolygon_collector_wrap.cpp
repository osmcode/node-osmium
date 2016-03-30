
// node-osmium
#include "node_osmium.hpp"
#include "multipolygon_collector_wrap.hpp"
#include "multipolygon_handler_wrap.hpp"
#include "handler.hpp"
#include "buffer_wrap.hpp"
#include "basic_reader_wrap.hpp"
#include "utils.hpp"
#include "apply.hpp"

namespace node_osmium {

    Nan::Persistent<v8::FunctionTemplate> MultipolygonCollectorWrap::constructor;

    void MultipolygonCollectorWrap::Initialize(v8::Local<v8::Object> target) {
        Nan::HandleScope scope;
        constructor = Nan::Persistent<v8::FunctionTemplate>::New(Nan::New(MultipolygonCollectorWrap::New));
        constructor->InstanceTemplate()->SetInternalFieldCount(1);
        constructor->SetClassName(symbol_MultipolygonCollector);
        Nan::SetPrototypeMethod(constructor, "read_relations", read_relations);
        Nan::SetPrototypeMethod(constructor, "handler", handler);
        target->Set(symbol_MultipolygonCollector, constructor->GetFunction());
    }

    v8::Local<v8::Value> MultipolygonCollectorWrap::New(const v8::Arguments& info) {
        Nan::HandleScope scope;

        if (!info.IsConstructCall()) {
            ThrowException(v8::Exception::Error(Nan::New("Cannot call constructor as function, you need to use 'new' keyword").ToLocalChecked()));
            return;
        }

        if (info.Length() != 0) {
            ThrowException(v8::Exception::TypeError(Nan::New("MultipolygonCollector is constructed without arguments").ToLocalChecked()));
            return;
        }

        try {
            MultipolygonCollectorWrap* multipolygon_collector_wrap = new MultipolygonCollectorWrap();
            multipolygon_collector_wrap->Wrap(info.This());
            info.GetReturnValue().Set(info.This());
            return;
        } catch (const std::exception& e) {
            ThrowException(v8::Exception::TypeError(Nan::New(e.what())));
            return;
        }

        info.GetReturnValue().Set(Nan::Undefined());
        return;
    }

    v8::Local<v8::Value> MultipolygonCollectorWrap::read_relations(const v8::Arguments& info) {
        INSTANCE_CHECK(MultipolygonCollectorWrap, "MultipolygonCollector", "read_relations");
        Nan::HandleScope scope;
        if (info.Length() != 1 || !info[0]->IsObject()) {
            ThrowException(v8::Exception::Error(Nan::New("call MultipolygonCollector.read_relation() with BasicReader or Buffer object").ToLocalChecked()));
            return;
        }
        try {
            if (BasicReaderWrap::constructor->HasInstance(info[0]->ToObject())) {
                osmium::io::Reader& reader = unwrap<BasicReaderWrap>(info[0]->ToObject());
                unwrap<MultipolygonCollectorWrap>(info.This()).read_relations(reader);
            } else if (BufferWrap::constructor->HasInstance(info[0]->ToObject())) {
                osmium::memory::Buffer& buffer = unwrap<BufferWrap>(info[0]->ToObject());
                unwrap<MultipolygonCollectorWrap>(info.This()).read_relations(buffer.begin(), buffer.end());
            } else {
                ThrowException(v8::Exception::Error(Nan::New("call MultipolygonCollector.read_relation() with BasicReader or Buffer object").ToLocalChecked()));
                return;
            }
        } catch (const std::exception& e) {
            std::string msg("osmium error: ");
            msg += e.what();
            ThrowException(v8::Exception::Error(Nan::New(msg).ToLocalChecked()));
            return;
        }
        info.GetReturnValue().Set(Nan::Undefined());
        return;
    }

    v8::Local<v8::Value> MultipolygonCollectorWrap::handler(const v8::Arguments& info) {
        INSTANCE_CHECK(MultipolygonCollectorWrap, "MultipolygonCollector", "handler");
        Nan::HandleScope scope;
        if (info.Length() != 1 || !info[0]->IsObject() || !JSHandler::constructor->HasInstance(info[0]->ToObject())) {
            ThrowException(v8::Exception::Error(Nan::New("call MultipolygonCollector.handler() with Handler object").ToLocalChecked()));
            return;
        }
        try {
            JSHandler& handler = unwrap<JSHandler>(info[0]->ToObject());
            auto& mc_handler = unwrap<MultipolygonCollectorWrap>(info.This()).handler([&handler](const osmium::memory::Buffer& area_buffer) {
                for (const osmium::OSMEntity& entity : area_buffer) {
                    handler.dispatch_entity(entity);
                }
            });
            info.GetReturnValue().Set(new_external<MultipolygonHandlerWrap>(mc_handler));
            return;

        } catch (const std::exception& e) {
            std::string msg("osmium error: ");
            msg += e.what();
            ThrowException(v8::Exception::Error(Nan::New(msg).ToLocalChecked()));
            return;
        }
        info.GetReturnValue().Set(Nan::Undefined());
        return;
    }

} // namespace node_osmium

