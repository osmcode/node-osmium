
// node
#include <node_buffer.h>
#include <node_version.h>

// osmium
#include <osmium/geom/wkb.hpp>
#include <osmium/geom/wkt.hpp>
#include <osmium/geom/geojson.hpp>

// node-osmium
#include "osm_area_wrap.hpp"

namespace node_osmium {

    extern osmium::geom::WKBFactory<> wkb_factory;
    extern osmium::geom::WKTFactory<> wkt_factory;
    extern osmium::geom::GeoJSONFactory<> geojson_factory;
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
        node::SetPrototypeMethod(constructor, "geojson", geojson);
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

    v8::Handle<v8::Value> OSMAreaWrap::geojson(const v8::Arguments& args) {
        INSTANCE_CHECK(OSMAreaWrap, "Area", "geojson");
        v8::HandleScope scope;

        try {
            std::string geojson { geojson_factory.create_multipolygon(wrapped(args.This())) };

            v8::Handle<v8::Context> context = v8::Context::GetCurrent();
            v8::Handle<v8::Object> global = context->Global();

            v8::Handle<v8::Object> JSON = global->Get(v8::String::New("JSON"))->ToObject();
            v8::Handle<v8::Function> JSON_parse = v8::Handle<v8::Function>::Cast(JSON->Get(v8::String::New("parse")));

            v8::Handle<v8::Value> jsonString = v8::String::New(geojson.c_str());
            return scope.Close(JSON_parse->Call(JSON, 1, &jsonString));
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

} // namespace node_osmium

