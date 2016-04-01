#ifndef FLEX_READER_WRAP_HPP
#define FLEX_READER_WRAP_HPP

#include "include_nan.hpp"

// osmium
#include <osmium/io/any_input.hpp>
#include <osmium/experimental/flex_reader.hpp>
#include <osmium/osm/entity_bits.hpp>
#include <osmium/osm/location.hpp>
#include <osmium/osm/types.hpp>

// node-osmium
#include "location_handler_wrap.hpp"

namespace node_osmium {

    typedef osmium::experimental::FlexReader<location_handler_type> flex_reader_type;

    class FlexReaderWrap : public Nan::ObjectWrap {

        static NAN_METHOD(header);
        static NAN_METHOD(close);
        static NAN_METHOD(read);
        static NAN_METHOD(read_all);

        flex_reader_type m_this;

    public:

        static Nan::Persistent<v8::FunctionTemplate> constructor;
        static void Initialize(v8::Local<v8::Object> target);
        static NAN_METHOD(New);

        FlexReaderWrap(const osmium::io::File& file, location_handler_type& location_handler, osmium::osm_entity_bits::type entities) :
            Nan::ObjectWrap(),
            m_this(file, location_handler, entities) {
        }

        flex_reader_type& get() {
            return m_this;
        }

    private:

        ~FlexReaderWrap() = default;

    }; // class FlexReaderWrap

} // namespace node_osmium

#endif // FLEX_READER_WRAP_HPP
