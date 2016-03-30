#ifndef FILE_WRAP_HPP
#define FILE_WRAP_HPP

// c++
#include <memory>
#include <string>

#include "include_nan.hpp"

// osmium
#include <osmium/io/file.hpp>

namespace node_osmium {

    class FileWrap : public Nan::ObjectWrap {

        osmium::io::File m_this;

    public:

        static Nan::Persistent<v8::FunctionTemplate> constructor;
        static void Initialize(v8::Handle<v8::Object> target);
        static NAN_METHOD(New);

        FileWrap(osmium::io::File&& file) :
            Nan::ObjectWrap(),
            m_this(std::move(file)) {
        }

        osmium::io::File& get() {
            return m_this;
        }

    private:

        ~FileWrap() = default;

    }; // class FileWrap

} // namespace node_osmium

#endif // FILE_WRAP_HPP
