
// osmium
#include <osmium/geom/wkb.hpp>
#include <osmium/geom/wkt.hpp>

// node-osmium
#include "osm_way_wrap.hpp"

namespace node_osmium {

    extern osmium::geom::WKBFactory<> wkb_factory;
    extern osmium::geom::WKTFactory<> wkt_factory;
    extern Nan::Persistent<v8::Object> module;

    Nan::Persistent<v8::FunctionTemplate> OSMWayWrap::constructor;

    void OSMWayWrap::Initialize(v8::Local<v8::Object> target) {
        Nan::HandleScope scope;
        v8::Local<v8::FunctionTemplate> lcons = Nan::New<v8::FunctionTemplate>(OSMWayWrap::New);
        constructor->Inherit(OSMWrappedObject::constructor);
        lcons->InstanceTemplate()->SetInternalFieldCount(1);
        lcons->SetClassName(Nan::New(symbol_Way));
        auto attributes = static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete);
        set_accessor(constructor, "type", get_type, attributes);
        set_accessor(constructor, "nodes_count", get_nodes_count, attributes);
        Nan::SetPrototypeMethod(lcons, "node_refs", node_refs);
        Nan::SetPrototypeMethod(lcons, "node_coordinates", node_coordinates);
        Nan::SetPrototypeMethod(lcons, "wkb", wkb);
        Nan::SetPrototypeMethod(lcons, "wkt", wkt);
        target->Set(Nan::New(symbol_Way), lcons->GetFunction());
    }

    NAN_METHOD(OSMWayWrap::New) {
        if (info.Length() == 1 && info[0]->IsExternal()) {
            v8::Local<v8::External> ext = v8::Local<v8::External>::Cast(info[0]);
            static_cast<OSMWayWrap*>(ext->Value())->Wrap(info.This());
            info.GetReturnValue().Set(info.This());
            return;
        } else {
            ThrowException(v8::Exception::TypeError(Nan::New("osmium.Way cannot be created in Javascript").ToLocalChecked()));
            return;
        }
    }

    NAN_METHOD(OSMWayWrap::wkb) {
        Nan::HandleScope scope;

        try {
            std::string wkb { wkb_factory.create_linestring(wrapped(info.This()), osmium::geom::use_nodes::unique) };
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

    NAN_METHOD(OSMWayWrap::wkt) {
        Nan::HandleScope scope;

        try {
            std::string wkt { wkt_factory.create_linestring(wrapped(info.This()), osmium::geom::use_nodes::unique) };
            info.GetReturnValue().Set(Nan::New(wkt).ToLocalChecked());
            return;
        } catch (std::runtime_error& e) {
            ThrowException(v8::Exception::Error(Nan::New(e.what())));
            return;
        }
    }

    NAN_METHOD(OSMWayWrap::get_nodes_count) {
        Nan::HandleScope scope;
        info.GetReturnValue().Set(Nan::New(wrapped(info.This()).nodes().size()));
        return;
    }

    NAN_METHOD(OSMWayWrap::node_refs) {
        Nan::HandleScope scope;

        const osmium::Way& way = wrapped(info.This());

        switch (info.Length()) {
            case 0: {
                v8::Local<v8::Array> nodes = Nan::New(way.nodes().size());
                int i = 0;
                for (const auto& node_ref : way.nodes()) {
                    nodes->Set(i, Nan::New(node_ref.ref()));
                    ++i;
                }
                info.GetReturnValue().Set(nodes);
                return;
            }
            case 1: {
                if (!info[0]->IsUint32()) {
                    ThrowException(v8::Exception::TypeError(Nan::New("call node_refs() without parameters or the index of the node you want").ToLocalChecked()));
                    return;
                }
                uint32_t n = info[0]->ToUint32()->Value();
                if (n < way.nodes().size()) {
                    info.GetReturnValue().Set(Nan::New(way.nodes()[n].ref()));
                    return;
                } else {
                    ThrowException(v8::Exception::RangeError(Nan::New("argument to node_refs() out of range").ToLocalChecked()));
                    return;
                }
            }
        }

        ThrowException(v8::Exception::TypeError(Nan::New("call node_refs() without parameters or the index of the node you want").ToLocalChecked()));
        return;
    }

    NAN_METHOD(OSMWayWrap::node_coordinates) {
        Nan::HandleScope scope;

        auto cf = module->Get(symbol_Coordinates);
        assert(cf->IsFunction());

        const osmium::Way& way = wrapped(info.This());

        if (way.nodes().size() < 2) {
            ThrowException(v8::Exception::Error(Nan::New("Way has no geometry").ToLocalChecked()));
            return;
        }

        switch (info.Length()) {
            case 0: {
                try {
                    v8::Local<v8::Array> nodes = Nan::New(0);
                    int i = 0;
                    osmium::Location last_location;
                    for (const auto& node_ref : way.nodes()) {
                        const osmium::Location location = node_ref.location();
                        if (location != last_location) {
                            v8::Local<v8::Value> argv[2] = { Nan::New(location.lon()), v8::Number::New(location.lat()) };
                            nodes->Set(i, v8::Local<v8::Function>::Cast(cf)->NewInstance(2, argv));
                            ++i;
                            last_location = location;
                        }
                    }
                    if (i < 2) {
                        ThrowException(v8::Exception::Error(Nan::New("Way has no geometry").ToLocalChecked()));
                        return;
                    }
                    info.GetReturnValue().Set(nodes);
                    return;
                } catch (osmium::invalid_location&) {
                    ThrowException(v8::Exception::TypeError(Nan::New("location of at least one of the nodes in this way not set").ToLocalChecked()));
                    return;
                }
            }
            case 1: {
                if (!info[0]->IsUint32()) {
                    ThrowException(v8::Exception::TypeError(Nan::New("call node_coordinates() without parameters or the index of the node you want").ToLocalChecked()));
                    return;
                }
                uint32_t n = info[0]->ToUint32()->Value();
                if (n < way.nodes().size()) {
                    const osmium::Location location = way.nodes()[n].location();
                    if (location.valid()) {
                        v8::Local<v8::Value> argv[2] = { Nan::New(location.lon()), v8::Number::New(location.lat()) };
                        info.GetReturnValue().Set(v8::Local<v8::Function>::Cast(cf)->NewInstance(2, argv));
                        return;
                    } else {
                        info.GetReturnValue().Set(Nan::Undefined());
                        return;
                    }
                } else {
                    ThrowException(v8::Exception::RangeError(Nan::New("argument to node_coordinates() out of range").ToLocalChecked()));
                    return;
                }
            }
        }

        ThrowException(v8::Exception::TypeError(Nan::New("call node_coordinates() without parameters or the index of the node you want").ToLocalChecked()));
        return;
    }

} // namespace node_osmium
