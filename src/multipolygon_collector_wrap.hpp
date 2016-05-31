#ifndef MULTIPOLYGON_COLLECTOR_WRAP_HPP
#define MULTIPOLYGON_COLLECTOR_WRAP_HPP

#include "include_nan.hpp"

// osmium
#include <osmium/area/multipolygon_collector.hpp>
#include <osmium/area/assembler.hpp>

namespace node_osmium {

    class MultipolygonCollectorWrap : public Nan::ObjectWrap {

        static NAN_METHOD(read_relations);
        static NAN_METHOD(handler);

        osmium::area::Assembler::config_type m_assembler_config;
        osmium::area::MultipolygonCollector<osmium::area::Assembler> m_collector;

        static osmium::area::Assembler::config_type default_assembler_config() noexcept {
            osmium::area::Assembler::config_type config;
            config.keep_type_tag = true;
            return config;
        }

    public:

        static Nan::Persistent<v8::FunctionTemplate> constructor;
        static void Initialize(v8::Local<v8::Object> target);
        static NAN_METHOD(New);

        MultipolygonCollectorWrap() :
            Nan::ObjectWrap(),
            m_assembler_config(default_assembler_config()),
            m_collector(m_assembler_config) {
        }

        osmium::area::MultipolygonCollector<osmium::area::Assembler>& get() {
            return m_collector;
        }

    private:

        ~MultipolygonCollectorWrap() = default;

    }; // class MultipolygonCollectorWrap

} // namespace node_osmium

#endif // MULTIPOLYGON_COLLECTOR_WRAP_HPP
