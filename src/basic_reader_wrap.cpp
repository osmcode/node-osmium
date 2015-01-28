
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

    extern v8::Persistent<v8::Object> module;

    v8::Persistent<v8::FunctionTemplate> BasicReaderWrap::constructor;

    void BasicReaderWrap::Initialize(v8::Handle<v8::Object> target) {
        v8::HandleScope scope;
        constructor = v8::Persistent<v8::FunctionTemplate>::New(v8::FunctionTemplate::New(BasicReaderWrap::New));
        constructor->InstanceTemplate()->SetInternalFieldCount(1);
        constructor->SetClassName(symbol_BasicReader);
        node::SetPrototypeMethod(constructor, "header", header);
        node::SetPrototypeMethod(constructor, "close", close);
        node::SetPrototypeMethod(constructor, "read", read);
        node::SetPrototypeMethod(constructor, "read_all", read_all);
        target->Set(symbol_BasicReader, constructor->GetFunction());
    }

    v8::Handle<v8::Value> BasicReaderWrap::New(const v8::Arguments& args) {
        v8::HandleScope scope;
        if (!args.IsConstructCall()) {
            return ThrowException(v8::Exception::Error(v8::String::New("Cannot call constructor as function, you need to use 'new' keyword")));
        }
        if (args.Length() < 1 || args.Length() > 2) {
            return ThrowException(v8::Exception::TypeError(v8::String::New("please provide a File object or string for the first argument and optional options v8::Object when creating a BasicReader")));
        }
        try {
            osmium::osm_entity_bits::type read_which_entities = osmium::osm_entity_bits::all;
            if (args.Length() == 2) {
                if (!args[1]->IsObject()) {
                    return ThrowException(v8::Exception::TypeError(v8::String::New("Second argument to BasicReader constructor must be object")));
                }
                read_which_entities = object_to_entity_bits(args[1]->ToObject());
            }
            if (args[0]->IsString()) {
                v8::String::Utf8Value filename { args[0] };
                osmium::io::File file(*filename);
                BasicReaderWrap* reader_wrap = new BasicReaderWrap(file, read_which_entities);
                reader_wrap->Wrap(args.This());
                return args.This();
            } else if (args[0]->IsObject() && FileWrap::constructor->HasInstance(args[0]->ToObject())) {
                v8::Local<v8::Object> file_obj = args[0]->ToObject();
                BasicReaderWrap* reader_wrap = new BasicReaderWrap(unwrap<FileWrap>(file_obj), read_which_entities);
                reader_wrap->Wrap(args.This());
                return args.This();
            } else {
                return ThrowException(v8::Exception::TypeError(v8::String::New("please provide a File object or string for the first argument when creating a BasicReader")));
            }
        } catch (const std::exception& ex) {
            return ThrowException(v8::Exception::TypeError(v8::String::New(ex.what())));
        }
        return scope.Close(v8::Undefined());
    }

    v8::Handle<v8::Value> BasicReaderWrap::header(const v8::Arguments& args) {
        INSTANCE_CHECK(BasicReaderWrap, "BasicReader", "header");
        v8::HandleScope scope;
        v8::Local<v8::Object> obj = v8::Object::New();
        const osmium::io::Header& header = unwrap<BasicReaderWrap>(args.This()).header();
        obj->Set(symbol_generator, v8::String::New(header.get("generator").c_str()));

        auto bounds_array = v8::Array::New(header.boxes().size());

        int i=0;
        for (const osmium::Box& box : header.boxes()) {
            bounds_array->Set(i++, create_js_box(box));
        }

        obj->Set(symbol_bounds, bounds_array);
        return scope.Close(obj);
    }

    v8::Handle<v8::Value> BasicReaderWrap::close(const v8::Arguments& args) {
        INSTANCE_CHECK(BasicReaderWrap, "BasicReader", "close");
        v8::HandleScope scope;
        try {
            unwrap<BasicReaderWrap>(args.This()).close();
        } catch (const std::exception& e) {
            std::string msg("osmium error: ");
            msg += e.what();
            return ThrowException(v8::Exception::Error(v8::String::New(msg.c_str())));
        }
        return scope.Close(v8::Undefined());
    }

    v8::Handle<v8::Value> BasicReaderWrap::read(const v8::Arguments& args) {
        INSTANCE_CHECK(BasicReaderWrap, "BasicReader", "read");
        v8::HandleScope scope;
        try {
            osmium::memory::Buffer buffer = unwrap<BasicReaderWrap>(args.This()).read();
            if (buffer) {
                return scope.Close(new_external<BufferWrap>(std::move(buffer)));
            }
        } catch (const std::exception& e) {
            std::string msg("osmium error: ");
            msg += e.what();
            return ThrowException(v8::Exception::Error(v8::String::New(msg.c_str())));
        }
        return scope.Close(v8::Undefined());
    }

    v8::Handle<v8::Value> BasicReaderWrap::read_all(const v8::Arguments& args) {
        INSTANCE_CHECK(BasicReaderWrap, "BasicReader", "read_all");
        osmium::memory::Buffer buffer(1024*1024, osmium::memory::Buffer::auto_grow::yes);
        v8::HandleScope scope;
        try {
            osmium::io::Reader& reader = unwrap<BasicReaderWrap>(args.This());
            while (osmium::memory::Buffer read_buffer = reader.read()) {
                buffer.add_buffer(read_buffer);
                buffer.commit();
            }
            if (buffer) {
                return scope.Close(new_external<BufferWrap>(std::move(buffer)));
            }
        } catch (const std::exception& e) {
            std::string msg("osmium error: ");
            msg += e.what();
            return ThrowException(v8::Exception::Error(v8::String::New(msg.c_str())));
        }
        return scope.Close(v8::Undefined());
    }

} // namespace node_osmium

