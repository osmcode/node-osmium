
// c++
#include <exception>

// node-osmium
#include "node_osmium.hpp"
#include "file_wrap.hpp"

namespace node_osmium {

    Nan::Persistent<v8::FunctionTemplate> FileWrap::constructor;

    void FileWrap::Initialize(v8::Local<v8::Object> target) {
        Nan::HandleScope scope;
        v8::Local<v8::FunctionTemplate> lcons = Nan::New<v8::FunctionTemplate>(FileWrap::New);
        lcons->InstanceTemplate()->SetInternalFieldCount(1);
        lcons->SetClassName(Nan::New(symbol_File));
        target->Set(Nan::New(symbol_File), lcons->GetFunction());
        constructor.Reset(lcons);
    }

    NAN_METHOD(FileWrap::New) {
        if (!info.IsConstructCall()) {
            Nan::ThrowError(Nan::New("Cannot call constructor as function, you need to use 'new' keyword").ToLocalChecked());
            return;
        }

        if (info.Length() == 0 || info.Length() > 2) {
            Nan::ThrowTypeError(Nan::New("File is constructed with one or two arguments: filename or node.Buffer and optional format").ToLocalChecked());
            return;
        }

        std::string format;
        if (info.Length() == 2) {
            if (!info[1]->IsString()) {
                Nan::ThrowTypeError(Nan::New("second argument to File constructor (format) must be a string").ToLocalChecked());
                return;
            }
            Nan::Utf8String format_string { info[1] };
            format = *format_string;
        }

        try {
            osmium::io::File file;

            if (info[0]->IsString()) {
                Nan::Utf8String filename { info[0] };
                file = osmium::io::File(*filename, format);
            } else if (info[0]->IsObject() && node::Buffer::HasInstance(info[0]->ToObject())) {
                auto source = info[0]->ToObject();
                file = osmium::io::File(node::Buffer::Data(source), node::Buffer::Length(source), format);
            } else {
                Nan::ThrowTypeError(Nan::New("first argument to File constructor must be a string (filename) or node.Buffer").ToLocalChecked());
                return;
            }

            file.check();
            FileWrap* file_wrap = new FileWrap(std::move(file));
            file_wrap->Wrap(info.This());
            info.GetReturnValue().Set(info.This());
            return;
        } catch (const std::exception& e) {
            Nan::ThrowTypeError(Nan::New(e.what()).ToLocalChecked());
            return;
        }

        info.GetReturnValue().Set(Nan::Undefined());
        return;
    }

} // namespace node_osmium

