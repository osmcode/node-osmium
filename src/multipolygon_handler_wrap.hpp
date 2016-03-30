#ifndef MULTIPOLYGON_HANDLER_WRAP_HPP
#define MULTIPOLYGON_HANDLER_WRAP_HPP

#include "include_nan.hpp"

// osmium
#include <osmium/area/multipolygon_collector.hpp>
#include <osmium/area/assembler.hpp>

namespace node_osmium {

    class MultipolygonHandlerWrap : public Nan::ObjectWrap {

        typedef osmium::area::MultipolygonCollector<osmium::area::Assembler>::HandlerPass2 handler_type;
        handler_type m_handler;

        static NAN_METHOD(stream_end);

    public:

        static Nan::Persistent<v8::FunctionTemplate> constructor;
        static void Initialize(v8::Handle<v8::Object> target);
        static NAN_METHOD(New);

        MultipolygonHandlerWrap(handler_type& handler) :
            Nan::ObjectWrap(),
            m_handler(handler) {
        }

        handler_type& get() {
            return m_handler;
        }

    private:

        ~MultipolygonHandlerWrap() = default;

    }; // class MultipolygonHandlerWrap

} // namespace node_osmium

#endif // MULTIPOLYGON_HANDLER_WRAP_HPP
