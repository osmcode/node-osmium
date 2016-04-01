
// osmium
#include <osmium/geom/wkb.hpp>
#include <osmium/geom/wkt.hpp>

// node-osmium
#include "osm_node_wrap.hpp"

namespace node_osmium {

    extern Nan::Persistent<v8::Object> module;
    extern osmium::geom::WKBFactory<> wkb_factory;
    extern osmium::geom::WKTFactory<> wkt_factory;

    Nan::Persistent<v8::FunctionTemplate> OSMNodeWrap::constructor;

    void OSMNodeWrap::Initialize(v8::Local<v8::Object> target) {
        Nan::HandleScope scope;
        v8::Local<v8::FunctionTemplate> lcons = Nan::New<v8::FunctionTemplate>(OSMNodeWrap::New);
        lcons->Inherit(Nan::New(OSMWrappedObject::constructor));
        lcons->InstanceTemplate()->SetInternalFieldCount(1);
        lcons->SetClassName(Nan::New(symbol_Node));
        Nan::SetPrototypeMethod(lcons, "wkb", wkb);
        Nan::SetPrototypeMethod(lcons, "wkt", wkt);
        ATTR(lcons, "type", get_type);
        ATTR(lcons, "location", get_coordinates);
        ATTR(lcons, "coordinates", get_coordinates);
        ATTR(lcons, "lon", get_lon);
        ATTR(lcons, "lat", get_lat);
        target->Set(Nan::New(symbol_Node), lcons->GetFunction());
        constructor.Reset(lcons);
    }

    NAN_METHOD(OSMNodeWrap::New) {
        if (info.Length() == 1 && info[0]->IsExternal()) {
            v8::Local<v8::External> ext = v8::Local<v8::External>::Cast(info[0]);
            static_cast<OSMNodeWrap*>(ext->Value())->Wrap(info.This());
            info.GetReturnValue().Set(info.This());
            return;
        } else {
            Nan::ThrowTypeError(Nan::New("osmium.Node cannot be created in Javascript").ToLocalChecked());
            return;
        }
    }

    NAN_GETTER(OSMNodeWrap::get_coordinates) {
        auto cf = Nan::New(module)->Get(Nan::New(symbol_Coordinates));
        assert(cf->IsFunction());

        const osmium::Location& location = wrapped(info.This()).location();
        if (!location) {
            info.GetReturnValue().Set(v8::Local<v8::Function>::Cast(cf)->NewInstance());
            return;
        }

        v8::Local<v8::Value> lon = Nan::New(location.lon_without_check());
        v8::Local<v8::Value> lat = Nan::New(location.lat_without_check());
        v8::Local<v8::Value> argv[2] = { lon, lat };
        info.GetReturnValue().Set(v8::Local<v8::Function>::Cast(cf)->NewInstance(2, argv));
        return;
    }

    NAN_GETTER(OSMNodeWrap::get_lon) {
        try {
            info.GetReturnValue().Set(Nan::New(wrapped(info.This()).location().lon()));
            return;
        } catch (osmium::invalid_location&) {
            info.GetReturnValue().Set(Nan::Undefined());
            return;
        }
    }

    NAN_GETTER(OSMNodeWrap::get_lat) {
        try {
            info.GetReturnValue().Set(Nan::New(wrapped(info.This()).location().lat()));
            return;
        } catch (osmium::invalid_location&) {
            info.GetReturnValue().Set(Nan::Undefined());
            return;
        }
    }

    NAN_METHOD(OSMNodeWrap::wkb) {
        INSTANCE_CHECK(OSMNodeWrap, "Node", "wkb");
        try {
            std::string wkb { wkb_factory.create_point(wrapped(info.This())) };
            info.GetReturnValue().Set(Nan::CopyBuffer(wkb.data(), wkb.size()).ToLocalChecked());
            return;
        } catch (std::runtime_error& e) {
            Nan::ThrowError(Nan::New(e.what()).ToLocalChecked());
            return;
        }
    }

    NAN_METHOD(OSMNodeWrap::wkt) {
        INSTANCE_CHECK(OSMNodeWrap, "Node", "wkt");
        try {
            std::string wkt { wkt_factory.create_point(wrapped(info.This())) };
            info.GetReturnValue().Set(Nan::New(wkt).ToLocalChecked());
            return;
        } catch (std::runtime_error& e) {
            Nan::ThrowError(Nan::New(e.what()).ToLocalChecked());
            return;
        }
    }

} // namespace node_osmium
