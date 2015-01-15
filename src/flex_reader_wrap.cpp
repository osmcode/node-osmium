
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
#include "flex_reader_wrap.hpp"
#include "utils.hpp"

namespace node_osmium {

    extern v8::Persistent<v8::Object> module;

    v8::Persistent<v8::FunctionTemplate> FlexReaderWrap::constructor;

    void FlexReaderWrap::Initialize(v8::Handle<v8::Object> target) {
        v8::HandleScope scope;
        constructor = v8::Persistent<v8::FunctionTemplate>::New(v8::FunctionTemplate::New(FlexReaderWrap::New));
        constructor->InstanceTemplate()->SetInternalFieldCount(1);
        constructor->SetClassName(symbol_FlexReader);
        node::SetPrototypeMethod(constructor, "header", header);
        node::SetPrototypeMethod(constructor, "close", close);
        node::SetPrototypeMethod(constructor, "read", read);
        node::SetPrototypeMethod(constructor, "read_all", read_all);
        target->Set(symbol_FlexReader, constructor->GetFunction());
    }

    v8::Handle<v8::Value> FlexReaderWrap::New(const v8::Arguments& args) {
        v8::HandleScope scope;
        if (!args.IsConstructCall()) {
            return ThrowException(v8::Exception::Error(v8::String::New("Cannot call constructor as function, you need to use 'new' keyword")));
        }
        if (args.Length() < 1 || args.Length() > 3) {
            return ThrowException(v8::Exception::TypeError(v8::String::New("please provide a File object or string for the first argument, a LocationHandler as second argument, and optional options v8::Object when creating a FlexReader")));
        }
        try {
            osmium::osm_entity_bits::type read_which_entities = osmium::osm_entity_bits::all;
            if (args.Length() == 3) {
                if (!args[1]->IsObject()) {
                    return ThrowException(v8::Exception::TypeError(v8::String::New("Third argument to FlexReader constructor must be object")));
                }
                read_which_entities = osmium::osm_entity_bits::nothing;
                v8::Local<v8::Object> options = args[1]->ToObject();

                v8::Local<v8::Value> want_nodes = options->Get(symbol_node);
                if (want_nodes->IsBoolean() && want_nodes->BooleanValue()) {
                    read_which_entities |= osmium::osm_entity_bits::node;
                }

                v8::Local<v8::Value> want_ways = options->Get(symbol_way);
                if (want_ways->IsBoolean() && want_ways->BooleanValue()) {
                    read_which_entities |= osmium::osm_entity_bits::way;
                }

                v8::Local<v8::Value> want_relations = options->Get(symbol_relation);
                if (want_relations->IsBoolean() && want_relations->BooleanValue()) {
                    read_which_entities |= osmium::osm_entity_bits::relation;
                }

                v8::Local<v8::Value> want_changesets = options->Get(symbol_changeset);
                if (want_changesets->IsBoolean() && want_changesets->BooleanValue()) {
                    read_which_entities |= osmium::osm_entity_bits::changeset;
                }

            }

            if (args[1]->IsObject() && LocationHandlerWrap::constructor->HasInstance(args[1]->ToObject())) {
                v8::Local<v8::Object> location_handler_obj = args[1]->ToObject();
                location_handler_type& location_handler = unwrap<LocationHandlerWrap>(location_handler_obj);
                if (args[0]->IsString()) {
                    osmium::io::File file(*v8::String::Utf8Value(args[0]));
                    FlexReaderWrap* reader_wrap = new FlexReaderWrap(file, location_handler, read_which_entities);
                    reader_wrap->Wrap(args.This());
                    return args.This();
                } else if (args[0]->IsObject() && FileWrap::constructor->HasInstance(args[0]->ToObject())) {
                    v8::Local<v8::Object> file_obj = args[0]->ToObject();
                    FlexReaderWrap* reader_wrap = new FlexReaderWrap(unwrap<FileWrap>(file_obj), location_handler, read_which_entities);
                    reader_wrap->Wrap(args.This());
                    return args.This();
                } else {
                    return ThrowException(v8::Exception::TypeError(v8::String::New("please provide a File object or string for the first argument when creating a FlexReader")));
                }
            } else {
                return ThrowException(v8::Exception::TypeError(v8::String::New("please provide a LocationHandler object for the second argument when creating a FlexReader")));
            }
        } catch (const std::exception& ex) {
            return ThrowException(v8::Exception::TypeError(v8::String::New(ex.what())));
        }
        return scope.Close(v8::Undefined());
    }

    v8::Handle<v8::Value> FlexReaderWrap::header(const v8::Arguments& args) {
        INSTANCE_CHECK(FlexReaderWrap, "FlexReader", "header");
        v8::HandleScope scope;
        v8::Local<v8::Object> obj = v8::Object::New();
        const osmium::io::Header& header = unwrap<FlexReaderWrap>(args.This()).header();
        obj->Set(symbol_generator, v8::String::New(header.get("generator").c_str()));

        auto bounds_array = v8::Array::New(header.boxes().size());

        int i=0;
        for (const osmium::Box& box : header.boxes()) {
            bounds_array->Set(i++, create_js_box(box));
        }

        obj->Set(symbol_bounds, bounds_array);
        return scope.Close(obj);
    }

    v8::Handle<v8::Value> FlexReaderWrap::close(const v8::Arguments& args) {
        INSTANCE_CHECK(FlexReaderWrap, "FlexReader", "close");
        v8::HandleScope scope;
        try {
            unwrap<FlexReaderWrap>(args.This()).close();
        } catch (const std::exception& e) {
            std::string msg("osmium error: ");
            msg += e.what();
            return ThrowException(v8::Exception::Error(v8::String::New(msg.c_str())));
        }
        return scope.Close(v8::Undefined());
    }

    v8::Handle<v8::Value> FlexReaderWrap::read(const v8::Arguments& args) {
        INSTANCE_CHECK(FlexReaderWrap, "FlexReader", "read");
        v8::HandleScope scope;
        try {
            osmium::memory::Buffer buffer = unwrap<FlexReaderWrap>(args.This()).read();
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

    v8::Handle<v8::Value> FlexReaderWrap::read_all(const v8::Arguments& args) {
        INSTANCE_CHECK(FlexReaderWrap, "FlexReader", "read_all");
        osmium::memory::Buffer buffer(1024*1024, osmium::memory::Buffer::auto_grow::yes);
        v8::HandleScope scope;
        try {
            flex_reader_type& reader = unwrap<FlexReaderWrap>(args.This());
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

