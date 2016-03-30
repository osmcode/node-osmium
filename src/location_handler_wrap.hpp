#ifndef LOCATION_HANDLER_WRAP_HPP
#define LOCATION_HANDLER_WRAP_HPP

// c++
#include <memory>
#include <string>

#include "include_nan.hpp"

// osmium
#include <osmium/index/map/all.hpp>
#include <osmium/handler/node_locations_for_ways.hpp>

namespace node_osmium {

    typedef osmium::index::map::Map<osmium::unsigned_object_id_type, osmium::Location> index_type;
    typedef osmium::handler::NodeLocationsForWays<index_type> location_handler_type;

    class LocationHandlerWrap : public node::ObjectWrap {

        std::unique_ptr<index_type> m_index;

        std::unique_ptr<location_handler_type> m_this;

        static v8::Handle<v8::Value> clear(const v8::Arguments& args);
        static v8::Handle<v8::Value> ignoreErrors(const v8::Arguments& args);
        static v8::Handle<v8::Value> stream_end(const v8::Arguments& args);

    public:

        static v8::Persistent<v8::FunctionTemplate> constructor;
        static void Initialize(v8::Handle<v8::Object> target);
        static v8::Handle<v8::Value> New(const v8::Arguments& args);

        LocationHandlerWrap(const std::string& cache_type) :
            ObjectWrap(),
            m_index(osmium::index::MapFactory<osmium::unsigned_object_id_type, osmium::Location>::instance().create_map(cache_type)),
            m_this(new location_handler_type(*m_index)) {
        }

        location_handler_type& get() {
            return *m_this;
        }

    private:

        ~LocationHandlerWrap() = default;

    }; // class LocationHandlerWrap

} // namespace node_osmium

#endif //  LOCATION_HANDLER_WRAP_HPP
