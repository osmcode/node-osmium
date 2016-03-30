
// osmium
#include <osmium/geom/wkb.hpp>
#include <osmium/geom/wkt.hpp>

// node-osmium
#include "osm_area_wrap.hpp"

namespace node_osmium {

    extern osmium::geom::WKBFactory<> wkb_factory;
    extern osmium::geom::WKTFactory<> wkt_factory;
    extern Nan::Persistent<v8::Object> module;

    Nan::Persistent<v8::FunctionTemplate> OSMAreaWrap::constructor;

    void OSMAreaWrap::Initialize(v8::Local<v8::Object> target) {
        Nan::HandleScope scope;
        constructor = Nan::Persistent<v8::FunctionTemplate>::New(Nan::New(OSMAreaWrap::New));
        constructor->Inherit(OSMWrappedObject::constructor);
        constructor->InstanceTemplate()->SetInternalFieldCount(1);
        constructor->SetClassName(symbol_Area);
        auto attributes = static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete);
        set_accessor(constructor, "type", get_type, attributes);
        set_accessor(constructor, "orig_id", get_orig_id, attributes);
        set_accessor(constructor, "from_way", from_way, attributes);
        Nan::SetPrototypeMethod(constructor, "wkb", wkb);
        Nan::SetPrototypeMethod(constructor, "wkt", wkt);
        Nan::SetPrototypeMethod(constructor, "coordinates", coordinates);
        target->Set(symbol_Area, constructor->GetFunction());
    }

    v8::Local<v8::Value> OSMAreaWrap::New(const v8::Arguments& info) {
        if (info.Length() == 1 && info[0]->IsExternal()) {
            v8::Local<v8::External> ext = v8::Local<v8::External>::Cast(info[0]);
            static_cast<OSMAreaWrap*>(ext->Value())->Wrap(info.This());
            return info.This();
        } else {
            ThrowException(v8::Exception::TypeError(Nan::New("osmium.Area cannot be created in Javascript").ToLocalChecked()));
            return;
        }
    }

    v8::Local<v8::Value> OSMAreaWrap::get_orig_id(v8::Local<v8::String> /* property */, const v8::AccessorInfo& info) {
        Nan::HandleScope scope;
        info.GetReturnValue().Set(Nan::New(wrapped(info.This()).orig_id()));
        return;
    }

    v8::Local<v8::Value> OSMAreaWrap::from_way(v8::Local<v8::String> /* property */, const v8::AccessorInfo& info) {
        return Nan::New(wrapped(info.This()).from_way());
    }

    v8::Local<v8::Value> OSMAreaWrap::wkb(const v8::Arguments& info) {
        INSTANCE_CHECK(OSMAreaWrap, "Area", "wkb");
        Nan::HandleScope scope;

        try {
            std::string wkb { wkb_factory.create_multipolygon(wrapped(info.This())) };
#if NODE_VERSION_AT_LEAST(0, 10, 0)
            info.GetReturnValue().Set(node::Buffer::New(wkb.data(), wkb.size())->handle_);
            return;
#else
            info.GetReturnValue().Set(node::Buffer::New(const_cast<char*>(wkb.data()), wkb.size())->handle_);
            return;
#endif
        } catch (std::runtime_error& e) {
            ThrowException(v8::Exception::Error(Nan::New(e.what())));
            return;
        }
    }

    v8::Local<v8::Value> OSMAreaWrap::wkt(const v8::Arguments& info) {
        INSTANCE_CHECK(OSMAreaWrap, "Area", "wkt");
        Nan::HandleScope scope;

        try {
            std::string wkt { wkt_factory.create_multipolygon(wrapped(info.This())) };
            info.GetReturnValue().Set(Nan::New(wkt).ToLocalChecked());
            return;
        } catch (std::runtime_error& e) {
            ThrowException(v8::Exception::Error(Nan::New(e.what())));
            return;
        }
    }

    v8::Local<v8::Array> get_coordinates(const osmium::NodeRefList& node_ref_list) {
        v8::Local<v8::Array> locations = Nan::New(node_ref_list.size());
        int i = 0;
        for (const auto& node_ref : node_ref_list) {
            const osmium::Location location = node_ref.location();
            v8::Local<v8::Array> coordinates = Nan::New(2);
            coordinates->Set(0, Nan::New(location.lon()));
            coordinates->Set(1, Nan::New(location.lat()));
            locations->Set(i, coordinates);
            ++i;
        }
        return locations;
    }

    v8::Local<v8::Value> OSMAreaWrap::coordinates(const v8::Arguments& info) {
        INSTANCE_CHECK(OSMAreaWrap, "Area", "coordinates");
        Nan::HandleScope scope;

        v8::Local<v8::Value> cf = module->Get(symbol_Coordinates);
        assert(cf->IsFunction());

        const osmium::Area& area = wrapped(info.This());
        auto num_rings = area.num_rings();

        if (num_rings.first == 0) {
            ThrowException(v8::Exception::Error(Nan::New("Area has no geometry").ToLocalChecked()));
            return;
        }

        v8::Local<v8::Array> rings = Nan::New(num_rings.first);

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

        info.GetReturnValue().Set(rings);
        return;
    }

} // namespace node_osmium

