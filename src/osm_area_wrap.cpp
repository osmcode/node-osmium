
// node
#include <node_buffer.h>
#include <node_version.h>

// osmium
#include <osmium/geom/wkb.hpp>
#include <osmium/geom/wkt.hpp>

// node-osmium
#include "osm_area_wrap.hpp"

namespace node_osmium {

    extern osmium::geom::WKBFactory<> wkb_factory;
    extern osmium::geom::WKTFactory<> wkt_factory;
    extern v8::Persistent<v8::Object> module;

    v8::Persistent<v8::FunctionTemplate> OSMAreaWrap::constructor;

    void OSMAreaWrap::Initialize(v8::Handle<v8::Object> target) {
        v8::HandleScope scope;
        constructor = v8::Persistent<v8::FunctionTemplate>::New(v8::FunctionTemplate::New(OSMAreaWrap::New));
        constructor->Inherit(OSMObjectWrap::constructor);
        constructor->InstanceTemplate()->SetInternalFieldCount(1);
        constructor->SetClassName(symbol_Area);
        auto attributes = static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete);
        set_accessor(constructor, "type", get_type, attributes);
        node::SetPrototypeMethod(constructor, "wkb", wkb);
        node::SetPrototypeMethod(constructor, "wkt", wkt);
        node::SetPrototypeMethod(constructor, "coordinates", coordinates);
        target->Set(symbol_Area, constructor->GetFunction());
    }

    v8::Handle<v8::Value> OSMAreaWrap::New(const v8::Arguments& args) {
        if (args.Length() == 1 && args[0]->IsExternal()) {
            v8::Local<v8::External> ext = v8::Local<v8::External>::Cast(args[0]);
            static_cast<OSMAreaWrap*>(ext->Value())->Wrap(args.This());
            return args.This();
        } else {
            return ThrowException(v8::Exception::TypeError(v8::String::New("osmium.Area cannot be created in Javascript")));
        }
    }

    v8::Handle<v8::Value> OSMAreaWrap::wkb(const v8::Arguments& args) {
        INSTANCE_CHECK(OSMAreaWrap, "Area", "wkb");
        v8::HandleScope scope;

        try {
            std::string wkb { wkb_factory.create_multipolygon(wrapped(args.This())) };
#if NODE_VERSION_AT_LEAST(0, 10, 0)
            return scope.Close(node::Buffer::New(wkb.data(), wkb.size())->handle_);
#else
            return scope.Close(node::Buffer::New(const_cast<char*>(wkb.data()), wkb.size())->handle_);
#endif
        } catch (std::runtime_error& e) {
            return ThrowException(v8::Exception::Error(v8::String::New(e.what())));
        }
    }

    v8::Handle<v8::Value> OSMAreaWrap::wkt(const v8::Arguments& args) {
        INSTANCE_CHECK(OSMAreaWrap, "Area", "wkt");
        v8::HandleScope scope;

        try {
            std::string wkt { wkt_factory.create_multipolygon(wrapped(args.This())) };
            return scope.Close(v8::String::New(wkt.c_str()));
        } catch (std::runtime_error& e) {
            return ThrowException(v8::Exception::Error(v8::String::New(e.what())));
        }
    }

    v8::Handle<v8::Array> get_coordinates(const osmium::NodeRefList& node_ref_list) {
        v8::Local<v8::Array> locations = v8::Array::New(node_ref_list.size());
        int i = 0;
        for (const auto& node_ref : node_ref_list) {
            const osmium::Location location = node_ref.location();
            v8::Local<v8::Array> coordinates = v8::Array::New(2);
            coordinates->Set(0, v8::Number::New(location.lon()));
            coordinates->Set(1, v8::Number::New(location.lat()));
            locations->Set(i, coordinates);
            ++i;
        }
        return locations;
    }

    v8::Handle<v8::Value> OSMAreaWrap::coordinates(const v8::Arguments& args) {
        INSTANCE_CHECK(OSMAreaWrap, "Area", "coordinates");
        v8::HandleScope scope;

        v8::Local<v8::Value> cf = module->Get(symbol_Coordinates);
        assert(cf->IsFunction());

        const osmium::Area& area = wrapped(args.This());
        auto num_rings = area.num_rings();

        if (num_rings.first == 0) {
            return ThrowException(v8::Exception::Error(v8::String::New("Area has no geometry")));
        }

        v8::Local<v8::Array> rings = v8::Array::New(num_rings.first);

        int n = 0;
        for (auto oit = area.cbegin<osmium::OuterRing>(); oit != area.cend<osmium::OuterRing>(); ++oit, ++n) {
            v8::Local<v8::Array> ring = v8::Array::New(
                1 + std::distance(area.inner_ring_cbegin(oit), area.inner_ring_cend(oit))
            );
            int m = 0;
            ring->Set(m++, get_coordinates(*oit));
            for (auto iit = area.inner_ring_cbegin(oit); iit != area.inner_ring_cend(oit); ++iit) {
                ring->Set(m++, get_coordinates(*iit));
            }
            rings->Set(n, ring);
        }

        return scope.Close(rings);
    }

} // namespace node_osmium

