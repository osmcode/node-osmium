#ifndef FLEX_READER_WRAP_HPP
#define FLEX_READER_WRAP_HPP

// v8/node
#include "include_v8.hpp"
#include <node_object_wrap.h>

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

    class FlexReaderWrap : public node::ObjectWrap {

        static v8::Handle<v8::Value> header(const v8::Arguments& args);
        static v8::Handle<v8::Value> close(const v8::Arguments& args);
        static v8::Handle<v8::Value> read(const v8::Arguments& args);
        static v8::Handle<v8::Value> read_all(const v8::Arguments& args);

        flex_reader_type m_this;

    public:

        static v8::Persistent<v8::FunctionTemplate> constructor;
        static void Initialize(v8::Handle<v8::Object> target);
        static v8::Handle<v8::Value> New(const v8::Arguments& args);

        FlexReaderWrap(const osmium::io::File& file, location_handler_type& location_handler, osmium::osm_entity_bits::type entities) :
            ObjectWrap(),
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
