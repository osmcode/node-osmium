
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
        v8::Local<v8::FunctionTemplate> lcons = Nan::New<v8::FunctionTemplate>(OSMAreaWrap::New);
        lcons->Inherit(Nan::New(OSMWrappedObject::constructor));
        lcons->InstanceTemplate()->SetInternalFieldCount(1);
        lcons->SetClassName(Nan::New(symbol_Area));
        ATTR(lcons, "type", get_type);
        ATTR(lcons, "orig_id", get_orig_id);
        ATTR(lcons, "from_way", get_from_way);
        Nan::SetPrototypeMethod(lcons, "wkb", wkb);
        Nan::SetPrototypeMethod(lcons, "wkt", wkt);
        Nan::SetPrototypeMethod(lcons, "coordinates", coordinates);
        target->Set(Nan::New(symbol_Area), lcons->GetFunction());
        constructor.Reset(lcons);
    }

    NAN_METHOD(OSMAreaWrap::New) {
        if (info.Length() == 1 && info[0]->IsExternal()) {
            v8::Local<v8::External> ext = v8::Local<v8::External>::Cast(info[0]);
            static_cast<OSMAreaWrap*>(ext->Value())->Wrap(info.This());
            info.GetReturnValue().Set(info.This());
            return;
        } else {
            Nan::ThrowTypeError(Nan::New("osmium.Area cannot be created in Javascript").ToLocalChecked());
            return;
        }
    }

    NAN_GETTER(OSMAreaWrap::get_orig_id) {
        info.GetReturnValue().Set(Nan::New<v8::Number>(wrapped(info.This()).orig_id()));
        return;
    }

    NAN_GETTER(OSMAreaWrap::get_from_way) {
        info.GetReturnValue().Set(Nan::New(wrapped(info.This()).from_way()));
        return;
    }

    NAN_METHOD(OSMAreaWrap::wkb) {
        INSTANCE_CHECK(OSMAreaWrap, "Area", "wkb");

        try {
            std::string wkb { wkb_factory.create_multipolygon(wrapped(info.This())) };
            info.GetReturnValue().Set(Nan::CopyBuffer(wkb.data(), wkb.size()).ToLocalChecked());
            return;
        } catch (std::runtime_error& e) {
            Nan::ThrowError(Nan::New(e.what()).ToLocalChecked());
            return;
        }
    }

    NAN_METHOD(OSMAreaWrap::wkt) {
        INSTANCE_CHECK(OSMAreaWrap, "Area", "wkt");

        try {
            std::string wkt { wkt_factory.create_multipolygon(wrapped(info.This())) };
            info.GetReturnValue().Set(Nan::New(wkt).ToLocalChecked());
            return;
        } catch (std::runtime_error& e) {
            Nan::ThrowError(Nan::New(e.what()).ToLocalChecked());
            return;
        }
    }

    v8::Local<v8::Array> get_coordinates(const osmium::NodeRefList& node_ref_list) {
        v8::Local<v8::Array> locations = Nan::New<v8::Array>(node_ref_list.size());
        int i = 0;
        for (const auto& node_ref : node_ref_list) {
            const osmium::Location location = node_ref.location();
            v8::Local<v8::Array> coordinates = Nan::New<v8::Array>(2);
            coordinates->Set(0, Nan::New(location.lon()));
            coordinates->Set(1, Nan::New(location.lat()));
            locations->Set(i, coordinates);
            ++i;
        }
        return locations;
    }

    NAN_METHOD(OSMAreaWrap::coordinates) {
        INSTANCE_CHECK(OSMAreaWrap, "Area", "coordinates");

        v8::Local<v8::Value> cf = Nan::New(module)->Get(Nan::New(symbol_Coordinates));
        assert(cf->IsFunction());

        const osmium::Area& area = wrapped(info.This());
        auto num_rings = area.num_rings();

        if (num_rings.first == 0) {
            Nan::ThrowError(Nan::New("Area has no geometry").ToLocalChecked());
            return;
        }

        v8::Local<v8::Array> rings = Nan::New<v8::Array>(num_rings.first);

        int n = 0;
        for (const auto& outer_ring : area.outer_rings()) {
            auto inner_rings_range = area.inner_rings(outer_ring);
            unsigned array_size = 1u + inner_rings_range.size();
            v8::Local<v8::Array> ring = Nan::New<v8::Array>(array_size);
            int m = 0;
            ring->Set(m++, get_coordinates(outer_ring));
            for (const auto& inner_ring : inner_rings_range) {
                ring->Set(m++, get_coordinates(inner_ring));
            }
            rings->Set(n, ring);
            ++n;
        }

        info.GetReturnValue().Set(rings);
        return;
    }

} // namespace node_osmium

