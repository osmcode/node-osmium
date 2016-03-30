
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
        v8::Local<v8::FunctionTemplate> lcons = Nan::New<v8::FunctionTemplate>(MultipolygonCollectorWrap::New);
        lcons->InstanceTemplate()->SetInternalFieldCount(1);
        lcons->SetClassName(Nan::New(symbol_MultipolygonCollector));
        Nan::SetPrototypeMethod(lcons, "read_relations", read_relations);
        Nan::SetPrototypeMethod(lcons, "handler", handler);
        target->Set(Nan::New(symbol_MultipolygonCollector), lcons->GetFunction());
        constructor.Reset(lcons);
    }

    NAN_METHOD(MultipolygonCollectorWrap::New) {
        Nan::HandleScope scope;

        if (!info.IsConstructCall()) {
            Nan::ThrowError(Nan::New("Cannot call constructor as function, you need to use 'new' keyword").ToLocalChecked());
            return;
        }

        if (info.Length() != 0) {
            Nan::ThrowTypeError(Nan::New("MultipolygonCollector is constructed without arguments").ToLocalChecked());
            return;
        }

        try {
            MultipolygonCollectorWrap* multipolygon_collector_wrap = new MultipolygonCollectorWrap();
            multipolygon_collector_wrap->Wrap(info.This());
            info.GetReturnValue().Set(info.This());
            return;
        } catch (const std::exception& e) {
            Nan::ThrowTypeError(Nan::New(e.what()).ToLocalChecked());
            return;
        }

        info.GetReturnValue().Set(Nan::Undefined());
        return;
    }

    NAN_METHOD(MultipolygonCollectorWrap::read_relations) {
        INSTANCE_CHECK(MultipolygonCollectorWrap, "MultipolygonCollector", "read_relations");
        Nan::HandleScope scope;
        if (info.Length() != 1 || !info[0]->IsObject()) {
            Nan::ThrowError(Nan::New("call MultipolygonCollector.read_relation() with BasicReader or Buffer object").ToLocalChecked());
            return;
        }
        try {
            if (Nan::New(BasicReaderWrap::constructor)->HasInstance(info[0]->ToObject())) {
                osmium::io::Reader& reader = unwrap<BasicReaderWrap>(info[0]->ToObject());
                unwrap<MultipolygonCollectorWrap>(info.This()).read_relations(reader);
            } else if (Nan::New(BufferWrap::constructor)->HasInstance(info[0]->ToObject())) {
                osmium::memory::Buffer& buffer = unwrap<BufferWrap>(info[0]->ToObject());
                unwrap<MultipolygonCollectorWrap>(info.This()).read_relations(buffer.begin(), buffer.end());
            } else {
                Nan::ThrowError(Nan::New("call MultipolygonCollector.read_relation() with BasicReader or Buffer object").ToLocalChecked());
                return;
            }
        } catch (const std::exception& e) {
            std::string msg("osmium error: ");
            msg += e.what();
            Nan::ThrowError(Nan::New(msg).ToLocalChecked());
            return;
        }
        info.GetReturnValue().Set(Nan::Undefined());
        return;
    }

    NAN_METHOD(MultipolygonCollectorWrap::handler) {
        INSTANCE_CHECK(MultipolygonCollectorWrap, "MultipolygonCollector", "handler");
        Nan::HandleScope scope;
        if (info.Length() != 1 || !info[0]->IsObject() || !Nan::New(JSHandler::constructor)->HasInstance(info[0]->ToObject())) {
            Nan::ThrowError(Nan::New("call MultipolygonCollector.handler() with Handler object").ToLocalChecked());
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
            Nan::ThrowError(Nan::New(msg).ToLocalChecked());
            return;
        }
        info.GetReturnValue().Set(Nan::Undefined());
        return;
    }

} // namespace node_osmium

