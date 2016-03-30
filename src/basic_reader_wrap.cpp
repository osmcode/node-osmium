
// c++
#include <exception>
#include <string>
#include <vector>

// node
#include <node.h>

// node-osmium
#include "node_osmium.hpp"
#include "buffer_wrap.hpp"
#include "file_wrap.hpp"
#include "handler.hpp"
#include "location_handler_wrap.hpp"
#include "osm_object_wrap.hpp"
#include "basic_reader_wrap.hpp"
#include "utils.hpp"

namespace node_osmium {

    extern Nan::Persistent<v8::Object> module;

    Nan::Persistent<v8::FunctionTemplate> BasicReaderWrap::constructor;

    void BasicReaderWrap::Initialize(v8::Local<v8::Object> target) {
        Nan::HandleScope scope;
        constructor = Nan::Persistent<v8::FunctionTemplate>::New(Nan::New(BasicReaderWrap::New));
        constructor->InstanceTemplate()->SetInternalFieldCount(1);
        constructor->SetClassName(symbol_BasicReader);
        Nan::SetPrototypeMethod(constructor, "header", header);
        Nan::SetPrototypeMethod(constructor, "close", close);
        Nan::SetPrototypeMethod(constructor, "read", read);
        Nan::SetPrototypeMethod(constructor, "read_all", read_all);
        target->Set(symbol_BasicReader, constructor->GetFunction());
    }

    v8::Local<v8::Value> BasicReaderWrap::New(const v8::Arguments& info) {
        Nan::HandleScope scope;
        if (!info.IsConstructCall()) {
            ThrowException(v8::Exception::Error(Nan::New("Cannot call constructor as function, you need to use 'new' keyword").ToLocalChecked()));
            return;
        }
        if (info.Length() < 1 || info.Length() > 2) {
            ThrowException(v8::Exception::TypeError(Nan::New("please provide a File object or string for the first argument and optional options v8::Object when creating a BasicReader").ToLocalChecked()));
            return;
        }
        try {
            osmium::osm_entity_bits::type read_which_entities = osmium::osm_entity_bits::all;
            if (info.Length() == 2) {
                if (!info[1]->IsObject()) {
                    ThrowException(v8::Exception::TypeError(Nan::New("Second argument to BasicReader constructor must be object").ToLocalChecked()));
                    return;
                }
                read_which_entities = object_to_entity_bits(info[1]->ToObject());
            }
            if (info[0]->IsString()) {
                v8::String::Utf8Value filename { info[0] };
                osmium::io::File file(*filename);
                BasicReaderWrap* reader_wrap = new BasicReaderWrap(file, read_which_entities);
                reader_wrap->Wrap(info.This());
                return info.This();
            } else if (info[0]->IsObject() && FileWrap::constructor->HasInstance(info[0]->ToObject())) {
                v8::Local<v8::Object> file_obj = info[0]->ToObject();
                BasicReaderWrap* reader_wrap = new BasicReaderWrap(unwrap<FileWrap>(file_obj), read_which_entities);
                reader_wrap->Wrap(info.This());
                return info.This();
            } else {
                ThrowException(v8::Exception::TypeError(Nan::New("please provide a File object or string for the first argument when creating a BasicReader").ToLocalChecked()));
                return;
            }
        } catch (const std::exception& ex) {
            ThrowException(v8::Exception::TypeError(Nan::New(ex.what())));
            return;
        }
        info.GetReturnValue().Set(Nan::Undefined());
        return;
    }

    v8::Local<v8::Value> BasicReaderWrap::header(const v8::Arguments& info) {
        INSTANCE_CHECK(BasicReaderWrap, "BasicReader", "header");
        Nan::HandleScope scope;
        v8::Local<v8::Object> obj = Nan::New();
        const osmium::io::Header& header = unwrap<BasicReaderWrap>(info.This()).header();
        obj->Set(symbol_generator, Nan::New(header.get("generator")).ToLocalChecked());

        auto bounds_array = Nan::New(header.boxes().size());

        int i=0;
        for (const osmium::Box& box : header.boxes()) {
            bounds_array->Set(i++, create_js_box(box));
        }

        obj->Set(symbol_bounds, bounds_array);
        info.GetReturnValue().Set(obj);
        return;
    }

    v8::Local<v8::Value> BasicReaderWrap::close(const v8::Arguments& info) {
        INSTANCE_CHECK(BasicReaderWrap, "BasicReader", "close");
        Nan::HandleScope scope;
        try {
            unwrap<BasicReaderWrap>(info.This()).close();
        } catch (const std::exception& e) {
            std::string msg("osmium error: ");
            msg += e.what();
            ThrowException(v8::Exception::Error(Nan::New(msg).ToLocalChecked()));
            return;
        }
        info.GetReturnValue().Set(Nan::Undefined());
        return;
    }

    v8::Local<v8::Value> BasicReaderWrap::read(const v8::Arguments& info) {
        INSTANCE_CHECK(BasicReaderWrap, "BasicReader", "read");
        Nan::HandleScope scope;
        try {
            osmium::memory::Buffer buffer = unwrap<BasicReaderWrap>(info.This()).read();
            if (buffer) {
                info.GetReturnValue().Set(new_external<BufferWrap>(std::move(buffer)));
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

    v8::Local<v8::Value> BasicReaderWrap::read_all(const v8::Arguments& info) {
        INSTANCE_CHECK(BasicReaderWrap, "BasicReader", "read_all");
        osmium::memory::Buffer buffer(1024*1024, osmium::memory::Buffer::auto_grow::yes);
        Nan::HandleScope scope;
        try {
            osmium::io::Reader& reader = unwrap<BasicReaderWrap>(info.This());
            while (osmium::memory::Buffer read_buffer = reader.read()) {
                buffer.add_buffer(read_buffer);
                buffer.commit();
            }
            if (buffer) {
                info.GetReturnValue().Set(new_external<BufferWrap>(std::move(buffer)));
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

} // namespace node_osmium

