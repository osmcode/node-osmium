
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

    void OSMNodeWrap::Initialize(v8::Handle<v8::Object> target) {
        Nan::HandleScope scope;
        constructor = Nan::Persistent<v8::FunctionTemplate>::New(Nan::New(OSMNodeWrap::New));
        constructor->Inherit(OSMWrappedObject::constructor);
        constructor->InstanceTemplate()->SetInternalFieldCount(1);
        constructor->SetClassName(symbol_Node);
        node::SetPrototypeMethod(constructor, "wkb", wkb);
        node::SetPrototypeMethod(constructor, "wkt", wkt);
        auto attributes = static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete);
        set_accessor(constructor, "type", get_type, attributes);
        set_accessor(constructor, "location", get_coordinates, attributes);
        set_accessor(constructor, "coordinates", get_coordinates, attributes);
        set_accessor(constructor, "lon", get_lon, attributes);
        set_accessor(constructor, "lat", get_lat, attributes);
        target->Set(symbol_Node, constructor->GetFunction());
    }

    v8::Handle<v8::Value> OSMNodeWrap::New(const v8::Arguments& info) {
        if (info.Length() == 1 && info[0]->IsExternal()) {
            v8::Local<v8::External> ext = v8::Local<v8::External>::Cast(info[0]);
            static_cast<OSMNodeWrap*>(ext->Value())->Wrap(info.This());
            return info.This();
        } else {
            return ThrowException(v8::Exception::TypeError(Nan::New("osmium.Node cannot be created in Javascript").ToLocalChecked()));
        }
    }

    v8::Handle<v8::Value> OSMNodeWrap::get_coordinates(v8::Local<v8::String> /* property */, const v8::AccessorInfo& info) {
        Nan::HandleScope scope;

        auto cf = module->Get(symbol_Coordinates);
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

    v8::Handle<v8::Value> OSMNodeWrap::get_lon(v8::Local<v8::String> /* property */, const v8::AccessorInfo& info) {
        Nan::HandleScope scope;
        try {
            info.GetReturnValue().Set(Nan::New(wrapped(info.This()).location().lon()));
            return;
        } catch (osmium::invalid_location&) {
            info.GetReturnValue().Set(Nan::Undefined());
            return;
        }
    }

    v8::Handle<v8::Value> OSMNodeWrap::get_lat(v8::Local<v8::String> /* property */, const v8::AccessorInfo& info) {
        Nan::HandleScope scope;
        try {
            info.GetReturnValue().Set(Nan::New(wrapped(info.This()).location().lat()));
            return;
        } catch (osmium::invalid_location&) {
            info.GetReturnValue().Set(Nan::Undefined());
            return;
        }
    }

    v8::Handle<v8::Value> OSMNodeWrap::wkb(const v8::Arguments& info) {
        INSTANCE_CHECK(OSMNodeWrap, "Node", "wkb");
        Nan::HandleScope scope;

        try {
            std::string wkb { wkb_factory.create_point(wrapped(info.This())) };
#if NODE_VERSION_AT_LEAST(0, 10, 0)
            info.GetReturnValue().Set(node::Buffer::New(wkb.data(), wkb.size())->handle_);
            return;
#else
            info.GetReturnValue().Set(node::Buffer::New(const_cast<char*>(wkb.data()), wkb.size())->handle_);
            return;
#endif
        } catch (std::runtime_error& e) {
            return ThrowException(v8::Exception::Error(Nan::New(e.what())));
        }
    }

    v8::Handle<v8::Value> OSMNodeWrap::wkt(const v8::Arguments& info) {
        INSTANCE_CHECK(OSMNodeWrap, "Node", "wkt");
        Nan::HandleScope scope;

        try {
            std::string wkt { wkt_factory.create_point(wrapped(info.This())) };
            info.GetReturnValue().Set(Nan::New(wkt).ToLocalChecked());
            return;
        } catch (std::runtime_error& e) {
            return ThrowException(v8::Exception::Error(Nan::New(e.what())));
        }
    }

} // namespace node_osmium
