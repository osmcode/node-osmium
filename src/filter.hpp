
// c++
#include <memory>

// nodejs
#include <node.h>

// osmium
#include <osmium/osm/entity_bits.hpp>
#include <osmium/osm/entity.hpp>
#include <osmium/tags/filter.hpp>

// node-osmium
#include "node_osmium.hpp"

namespace node_osmium {

    class Filter {

        static std::vector<std::unique_ptr<Filter>> all_filters;

        osmium::osm_entity_bits::type m_entity_bits;
        osmium::osm_entity_bits::type m_tagged_entity_bits;
        osmium::tags::KeyValueFilter m_filters[5];

        void setup_filter(v8::Array* array, osmium::item_type item_type);

    public:

        static void init_filters();
        static v8::Handle<v8::Value> register_filter(const v8::Arguments& args);
        static const Filter& get_filter(size_t id);

        Filter();
        Filter(v8::Handle<v8::Object> object);

        bool match(const osmium::OSMEntity& entity) const;

    }; // class Filter

} // namespace node_osmium

