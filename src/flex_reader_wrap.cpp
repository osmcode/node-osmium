
// c++
#include <exception>
#include <string>
#include <vector>

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

    extern Nan::Persistent<v8::Object> module;

    Nan::Persistent<v8::FunctionTemplate> FlexReaderWrap::constructor;

    void FlexReaderWrap::Initialize(v8::Local<v8::Object> target) {
        Nan::HandleScope scope;
        constructor = Nan::Persistent<v8::FunctionTemplate>::New(Nan::New(FlexReaderWrap::New));
        constructor->InstanceTemplate()->SetInternalFieldCount(1);
        constructor->SetClassName(symbol_FlexReader);
        Nan::SetPrototypeMethod(constructor, "header", header);
        Nan::SetPrototypeMethod(constructor, "close", close);
        Nan::SetPrototypeMethod(constructor, "read", read);
        Nan::SetPrototypeMethod(constructor, "read_all", read_all);
        target->Set(symbol_FlexReader, constructor->GetFunction());
    }

    v8::Local<v8::Value> FlexReaderWrap::New(const v8::Arguments& info) {
        Nan::HandleScope scope;
        if (!info.IsConstructCall()) {
            return ThrowException(v8::Exception::Error(Nan::New("Cannot call constructor as function, you need to use 'new' keyword").ToLocalChecked()));
        }
        if (info.Length() < 2 || info.Length() > 3) {
            return ThrowException(v8::Exception::TypeError(Nan::New("please provide a File object or string for the first argument, a LocationHandler as second argument, and optional options v8::Object when creating a FlexReader").ToLocalChecked()));
        }
        try {
            osmium::osm_entity_bits::type read_which_entities = osmium::osm_entity_bits::nwra;
            if (info.Length() == 3 && !info[2]->IsUndefined()) {
                if (!info[2]->IsObject()) {
                    return ThrowException(v8::Exception::TypeError(Nan::New("Third argument to FlexReader constructor must be object").ToLocalChecked()));
                }
                read_which_entities = object_to_entity_bits(info[2]->ToObject());
            }

            if (info[1]->IsObject() && LocationHandlerWrap::constructor->HasInstance(info[1]->ToObject())) {
                v8::Local<v8::Object> location_handler_obj = info[1]->ToObject();
                location_handler_type& location_handler = unwrap<LocationHandlerWrap>(location_handler_obj);
                if (info[0]->IsString()) {
                    v8::String::Utf8Value filename { info[0] };
                    osmium::io::File file(*filename);
                    FlexReaderWrap* reader_wrap = new FlexReaderWrap(file, location_handler, read_which_entities);
                    reader_wrap->Wrap(info.This());
                    return info.This();
                } else if (info[0]->IsObject() && FileWrap::constructor->HasInstance(info[0]->ToObject())) {
                    v8::Local<v8::Object> file_obj = info[0]->ToObject();
                    FlexReaderWrap* reader_wrap = new FlexReaderWrap(unwrap<FileWrap>(file_obj), location_handler, read_which_entities);
                    reader_wrap->Wrap(info.This());
                    return info.This();
                } else {
                    return ThrowException(v8::Exception::TypeError(Nan::New("please provide a File object or string for the first argument when creating a FlexReader").ToLocalChecked()));
                }
            } else {
                return ThrowException(v8::Exception::TypeError(Nan::New("please provide a LocationHandler object for the second argument when creating a FlexReader").ToLocalChecked()));
            }
        } catch (const std::exception& ex) {
            return ThrowException(v8::Exception::TypeError(Nan::New(ex.what())));
        }
        info.GetReturnValue().Set(Nan::Undefined());
        return;
    }

    v8::Local<v8::Value> FlexReaderWrap::header(const v8::Arguments& info) {
        INSTANCE_CHECK(FlexReaderWrap, "FlexReader", "header");
        Nan::HandleScope scope;
        v8::Local<v8::Object> obj = Nan::New();
        const osmium::io::Header& header = unwrap<FlexReaderWrap>(info.This()).header();
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

    v8::Local<v8::Value> FlexReaderWrap::close(const v8::Arguments& info) {
        INSTANCE_CHECK(FlexReaderWrap, "FlexReader", "close");
        Nan::HandleScope scope;
        try {
            unwrap<FlexReaderWrap>(info.This()).close();
        } catch (const std::exception& e) {
            std::string msg("osmium error: ");
            msg += e.what();
            return ThrowException(v8::Exception::Error(Nan::New(msg).ToLocalChecked()));
        }
        info.GetReturnValue().Set(Nan::Undefined());
        return;
    }

    v8::Local<v8::Value> FlexReaderWrap::read(const v8::Arguments& info) {
        INSTANCE_CHECK(FlexReaderWrap, "FlexReader", "read");
        Nan::HandleScope scope;
        try {
            osmium::memory::Buffer buffer = unwrap<FlexReaderWrap>(info.This()).read();
            if (buffer) {
                info.GetReturnValue().Set(new_external<BufferWrap>(std::move(buffer)));
                return;
            }
        } catch (const std::exception& e) {
            std::string msg("osmium error: ");
            msg += e.what();
            return ThrowException(v8::Exception::Error(Nan::New(msg).ToLocalChecked()));
        }
        info.GetReturnValue().Set(Nan::Undefined());
        return;
    }

    v8::Local<v8::Value> FlexReaderWrap::read_all(const v8::Arguments& info) {
        INSTANCE_CHECK(FlexReaderWrap, "FlexReader", "read_all");
        osmium::memory::Buffer buffer(1024*1024, osmium::memory::Buffer::auto_grow::yes);
        Nan::HandleScope scope;
        try {
            flex_reader_type& reader = unwrap<FlexReaderWrap>(info.This());
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
            return ThrowException(v8::Exception::Error(Nan::New(msg).ToLocalChecked()));
        }
        info.GetReturnValue().Set(Nan::Undefined());
        return;
    }

} // namespace node_osmium

