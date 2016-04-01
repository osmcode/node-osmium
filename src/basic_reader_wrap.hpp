#ifndef BASIC_READER_WRAP_HPP
#define BASIC_READER_WRAP_HPP

#include "include_nan.hpp"

// osmium
#include <osmium/io/any_input.hpp>
#include <osmium/io/reader.hpp>
#include <osmium/osm/entity_bits.hpp>
#include <osmium/osm/location.hpp>
#include <osmium/osm/types.hpp>

namespace node_osmium {

    class BasicReaderWrap : public Nan::ObjectWrap {

        static NAN_METHOD(header);
        static NAN_METHOD(close);
        static NAN_METHOD(read);
        static NAN_METHOD(read_all);

        osmium::io::Reader m_this;

    public:

        static Nan::Persistent<v8::FunctionTemplate> constructor;
        static void Initialize(v8::Local<v8::Object> target);
        static NAN_METHOD(New);

        BasicReaderWrap(const osmium::io::File& file, osmium::osm_entity_bits::type entities) :
            Nan::ObjectWrap(),
            m_this(file, entities) {
        }

        osmium::io::Reader& get() {
            return m_this;
        }

    private:

        ~BasicReaderWrap() = default;

    }; // class BasicReaderWrap

} // namespace node_osmium

#endif // BASIC_READER_WRAP_HPP
