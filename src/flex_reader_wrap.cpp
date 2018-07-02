
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
        v8::Local<v8::FunctionTemplate> lcons = Nan::New<v8::FunctionTemplate>(FlexReaderWrap::New);
        lcons->InstanceTemplate()->SetInternalFieldCount(1);
        lcons->SetClassName(Nan::New(symbol_FlexReader));
        Nan::SetPrototypeMethod(lcons, "header", header);
        Nan::SetPrototypeMethod(lcons, "close", close);
        Nan::SetPrototypeMethod(lcons, "read", read);
        Nan::SetPrototypeMethod(lcons, "read_all", read_all);
        target->Set(Nan::New(symbol_FlexReader), lcons->GetFunction());
        constructor.Reset(lcons);
    }

    NAN_METHOD(FlexReaderWrap::New) {
        if (!info.IsConstructCall()) {
            Nan::ThrowError(Nan::New("Cannot call constructor as function, you need to use 'new' keyword").ToLocalChecked());
            return;
        }
        if (info.Length() < 2 || info.Length() > 3) {
            Nan::ThrowTypeError(Nan::New("please provide a File object or string for the first argument, a LocationHandler as second argument, and optional options v8::Object when creating a FlexReader").ToLocalChecked());
            return;
        }
        try {
            osmium::osm_entity_bits::type read_which_entities = osmium::osm_entity_bits::nwra;
            if (info.Length() == 3 && !info[2]->IsUndefined()) {
                if (!info[2]->IsObject()) {
                    Nan::ThrowTypeError(Nan::New("Third argument to FlexReader constructor must be object").ToLocalChecked());
                    return;
                }
                read_which_entities = object_to_entity_bits(info[2]->ToObject());
            }

            if (info[1]->IsObject() && Nan::New(LocationHandlerWrap::constructor)->HasInstance(info[1]->ToObject())) {
                v8::Local<v8::Object> location_handler_obj = info[1]->ToObject();
                location_handler_type& location_handler = unwrap<LocationHandlerWrap>(location_handler_obj);
                if (info[0]->IsString()) {
                    Nan::Utf8String filename { info[0] };
                    osmium::io::File file(*filename);
                    FlexReaderWrap* reader_wrap = new FlexReaderWrap(file, location_handler, read_which_entities);
                    reader_wrap->Wrap(info.This());
                    info.GetReturnValue().Set(info.This());
                    return;
                } else if (info[0]->IsObject() && Nan::New(FileWrap::constructor)->HasInstance(info[0]->ToObject())) {
                    v8::Local<v8::Object> file_obj = info[0]->ToObject();
                    FlexReaderWrap* reader_wrap = new FlexReaderWrap(unwrap<FileWrap>(file_obj), location_handler, read_which_entities);
                    reader_wrap->Wrap(info.This());
                    info.GetReturnValue().Set(info.This());
                    return;
                } else {
                    Nan::ThrowTypeError(Nan::New("please provide a File object or string for the first argument when creating a FlexReader").ToLocalChecked());
                    return;
                }
            } else {
                Nan::ThrowTypeError(Nan::New("please provide a LocationHandler object for the second argument when creating a FlexReader").ToLocalChecked());
                return;
            }
        } catch (const std::exception& ex) {
            Nan::ThrowTypeError(Nan::New(ex.what()).ToLocalChecked());
            return;
        }
        info.GetReturnValue().Set(Nan::Undefined());
        return;
    }

    NAN_METHOD(FlexReaderWrap::header) {
        INSTANCE_CHECK(FlexReaderWrap, "FlexReader", "header");
        v8::Local<v8::Object> obj = Nan::New<v8::Object>();
        const osmium::io::Header& header = unwrap<FlexReaderWrap>(info.This()).header();
        obj->Set(Nan::New(symbol_generator), Nan::New(header.get("generator")).ToLocalChecked());

        auto bounds_array = Nan::New<v8::Array>(static_cast<unsigned int>(header.boxes().size()));

        int i=0;
        for (const osmium::Box& box : header.boxes()) {
            bounds_array->Set(i++, create_js_box(box));
        }

        obj->Set(Nan::New(symbol_bounds), bounds_array);
        info.GetReturnValue().Set(obj);
        return;
    }

    NAN_METHOD(FlexReaderWrap::close) {
        INSTANCE_CHECK(FlexReaderWrap, "FlexReader", "close");
        try {
            unwrap<FlexReaderWrap>(info.This()).close();
        } catch (const std::exception& e) {
            std::string msg("osmium error: ");
            msg += e.what();
            Nan::ThrowError(Nan::New(msg).ToLocalChecked());
            return;
        }
        info.GetReturnValue().Set(Nan::Undefined());
        return;
    }

    NAN_METHOD(FlexReaderWrap::read) {
        INSTANCE_CHECK(FlexReaderWrap, "FlexReader", "read");
        try {
            osmium::memory::Buffer buffer = unwrap<FlexReaderWrap>(info.This()).read();
            if (buffer) {
                info.GetReturnValue().Set(new_external<BufferWrap>(std::move(buffer)));
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

    NAN_METHOD(FlexReaderWrap::read_all) {
        INSTANCE_CHECK(FlexReaderWrap, "FlexReader", "read_all");
        osmium::memory::Buffer buffer(1024*1024, osmium::memory::Buffer::auto_grow::yes);
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
            Nan::ThrowError(Nan::New(msg).ToLocalChecked());
            return;
        }
        info.GetReturnValue().Set(Nan::Undefined());
        return;
    }

} // namespace node_osmium

