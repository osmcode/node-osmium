
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
        lcons->Inherit(Nan::New(OSMWrappedObject::constructor));
        lcons->InstanceTemplate()->SetInternalFieldCount(1);
        lcons->SetClassName(Nan::New(symbol_Way));
        ATTR(lcons, "type", get_type);
        ATTR(lcons, "nodes_count", get_nodes_count);
        Nan::SetPrototypeMethod(lcons, "node_refs", node_refs);
        Nan::SetPrototypeMethod(lcons, "node_coordinates", node_coordinates);
        Nan::SetPrototypeMethod(lcons, "wkb", wkb);
        Nan::SetPrototypeMethod(lcons, "wkt", wkt);
        target->Set(Nan::New(symbol_Way), lcons->GetFunction());
        constructor.Reset(lcons);
    }

    NAN_METHOD(OSMWayWrap::New) {
        if (info.Length() == 1 && info[0]->IsExternal()) {
            v8::Local<v8::External> ext = v8::Local<v8::External>::Cast(info[0]);
            static_cast<OSMWayWrap*>(ext->Value())->Wrap(info.This());
            info.GetReturnValue().Set(info.This());
            return;
        } else {
            Nan::ThrowTypeError(Nan::New("osmium.Way cannot be created in Javascript").ToLocalChecked());
            return;
        }
    }

    NAN_METHOD(OSMWayWrap::wkb) {

        try {
            std::string wkb { wkb_factory.create_linestring(wrapped(info.This()), osmium::geom::use_nodes::unique) };
            info.GetReturnValue().Set(Nan::CopyBuffer(wkb.data(), wkb.size()).ToLocalChecked());
            return;
        } catch (std::runtime_error& e) {
            Nan::ThrowError(Nan::New(e.what()).ToLocalChecked());
            return;
        }
    }

    NAN_METHOD(OSMWayWrap::wkt) {

        try {
            std::string wkt { wkt_factory.create_linestring(wrapped(info.This()), osmium::geom::use_nodes::unique) };
            info.GetReturnValue().Set(Nan::New(wkt).ToLocalChecked());
            return;
        } catch (std::runtime_error& e) {
            Nan::ThrowError(Nan::New(e.what()).ToLocalChecked());
            return;
        }
    }

    NAN_GETTER(OSMWayWrap::get_nodes_count) {
        info.GetReturnValue().Set(Nan::New<v8::Number>(wrapped(info.This()).nodes().size()));
        return;
    }

    NAN_METHOD(OSMWayWrap::node_refs) {

        const osmium::Way& way = wrapped(info.This());

        switch (info.Length()) {
            case 0: {
                v8::Local<v8::Array> nodes = Nan::New<v8::Array>(way.nodes().size());
                int i = 0;
                for (const auto& node_ref : way.nodes()) {
                    nodes->Set(i, Nan::New<v8::Number>(node_ref.ref()));
                    ++i;
                }
                info.GetReturnValue().Set(nodes);
                return;
            }
            case 1: {
                if (!info[0]->IsUint32()) {
                    Nan::ThrowTypeError(Nan::New("call node_refs() without parameters or the index of the node you want").ToLocalChecked());
                    return;
                }
                uint32_t n = info[0]->Uint32Value();
                if (n < way.nodes().size()) {
                    info.GetReturnValue().Set(Nan::New<v8::Number>(way.nodes()[n].ref()));
                    return;
                } else {
                    Nan::ThrowRangeError(Nan::New("argument to node_refs() out of range").ToLocalChecked());
                    return;
                }
            }
        }

        Nan::ThrowTypeError(Nan::New("call node_refs() without parameters or the index of the node you want").ToLocalChecked());
        return;
    }

    NAN_METHOD(OSMWayWrap::node_coordinates) {

        auto cf = Nan::New(module)->Get(Nan::New(symbol_Coordinates));
        assert(cf->IsFunction());

        const osmium::Way& way = wrapped(info.This());

        if (way.nodes().size() < 2) {
            Nan::ThrowError(Nan::New("Way has no geometry").ToLocalChecked());
            return;
        }

        switch (info.Length()) {
            case 0: {
                try {
                    v8::Local<v8::Array> nodes = Nan::New<v8::Array>(0);
                    int i = 0;
                    osmium::Location last_location;
                    for (const auto& node_ref : way.nodes()) {
                        const osmium::Location location = node_ref.location();
                        if (location != last_location) {
                            v8::Local<v8::Value> argv[2] = { Nan::New(location.lon()), Nan::New<v8::Number>(location.lat()) };
                            Nan::MaybeLocal<v8::Object> maybe_local = Nan::NewInstance(v8::Local<v8::Function>::Cast(cf), 2, argv);
                            if (maybe_local.IsEmpty()) Nan::ThrowError("Could not create new Node instance");
                            nodes->Set(i, maybe_local.ToLocalChecked()->ToObject());
                            ++i;
                            last_location = location;
                        }
                    }
                    if (i < 2) {
                        Nan::ThrowError(Nan::New("Way has no geometry").ToLocalChecked());
                        return;
                    }
                    info.GetReturnValue().Set(nodes);
                    return;
                } catch (osmium::invalid_location&) {
                    Nan::ThrowTypeError(Nan::New("location of at least one of the nodes in this way not set").ToLocalChecked());
                    return;
                }
            }
            case 1: {
                if (!info[0]->IsUint32()) {
                    Nan::ThrowTypeError(Nan::New("call node_coordinates() without parameters or the index of the node you want").ToLocalChecked());
                    return;
                }
                uint32_t n = info[0]->Uint32Value();
                if (n < way.nodes().size()) {
                    const osmium::Location location = way.nodes()[n].location();
                    if (location.valid()) {
                        v8::Local<v8::Value> argv[2] = { Nan::New(location.lon()), Nan::New<v8::Number>(location.lat()) };
                        Nan::MaybeLocal<v8::Object> maybe_local = Nan::NewInstance(v8::Local<v8::Function>::Cast(cf), 2, argv);
                        if (maybe_local.IsEmpty()) Nan::ThrowError("Could not create new Node instance");
                        info.GetReturnValue().Set(maybe_local.ToLocalChecked()->ToObject());
                        return;
                    } else {
                        info.GetReturnValue().Set(Nan::Undefined());
                        return;
                    }
                } else {
                    Nan::ThrowRangeError(Nan::New("argument to node_coordinates() out of range").ToLocalChecked());
                    return;
                }
            }
        }

        Nan::ThrowTypeError(Nan::New("call node_coordinates() without parameters or the index of the node you want").ToLocalChecked());
        return;
    }

} // namespace node_osmium
