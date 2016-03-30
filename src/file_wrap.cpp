
// c++
#include <exception>

// node-osmium
#include "node_osmium.hpp"
#include "file_wrap.hpp"

namespace node_osmium {

    Nan::Persistent<v8::FunctionTemplate> FileWrap::constructor;

    void FileWrap::Initialize(v8::Handle<v8::Object> target) {
        Nan::HandleScope scope;
        constructor = Nan::Persistent<v8::FunctionTemplate>::New(Nan::New(FileWrap::New));
        constructor->InstanceTemplate()->SetInternalFieldCount(1);
        constructor->SetClassName(symbol_File);
        target->Set(symbol_File, constructor->GetFunction());
    }

    v8::Handle<v8::Value> FileWrap::New(const v8::Arguments& info) {
        Nan::HandleScope scope;

        if (!info.IsConstructCall()) {
            return ThrowException(v8::Exception::Error(Nan::New("Cannot call constructor as function, you need to use 'new' keyword").ToLocalChecked()));
        }

        if (info.Length() == 0 || info.Length() > 2) {
            return ThrowException(v8::Exception::TypeError(Nan::New("File is constructed with one or two arguments: filename or node.Buffer and optional format").ToLocalChecked()));
        }

        std::string format;
        if (info.Length() == 2) {
            if (!info[1]->IsString()) {
                return ThrowException(v8::Exception::TypeError(Nan::New("second argument to File constructor (format) must be a string").ToLocalChecked()));
            }
            v8::String::Utf8Value format_string { info[1] };
            format = *format_string;
        }

        try {
            osmium::io::File file;

            if (info[0]->IsString()) {
                v8::String::Utf8Value filename { info[0] };
                file = osmium::io::File(*filename, format);
            } else if (info[0]->IsObject() && node::Buffer::HasInstance(info[0]->ToObject())) {
                auto source = info[0]->ToObject();
                file = osmium::io::File(node::Buffer::Data(source), node::Buffer::Length(source), format);
            } else {
                return ThrowException(v8::Exception::TypeError(Nan::New("first argument to File constructor must be a string (filename) or node.Buffer").ToLocalChecked()));
            }

            file.check();
            FileWrap* file_wrap = new FileWrap(std::move(file));
            file_wrap->Wrap(info.This());
            return info.This();
        } catch (const std::exception& e) {
            return ThrowException(v8::Exception::TypeError(Nan::New(e.what())));
        }

        info.GetReturnValue().Set(Nan::Undefined());
        return;
    }

} // namespace node_osmium

