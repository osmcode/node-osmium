
// c++
#include <string>

// osmium
#include <osmium/osm/object.hpp>

// node-osmium
#include "node_osmium.hpp"
#include "handler.hpp"
#include "osm_node_wrap.hpp"
#include "osm_way_wrap.hpp"
#include "osm_relation_wrap.hpp"
#include "osm_area_wrap.hpp"
#include "osm_changeset_wrap.hpp"
#include "utils.hpp"

namespace node_osmium {

    Nan::Persistent<v8::FunctionTemplate> JSHandler::constructor;
    Nan::Persistent<v8::String> JSHandler::symbol_tagged_nodes_only;

    void JSHandler::Initialize(v8::Local<v8::Object> target) {
        Nan::HandleScope scope;
        constructor = Nan::Persistent<v8::FunctionTemplate>::New(Nan::New(JSHandler::New));
        constructor->InstanceTemplate()->SetInternalFieldCount(1);
        constructor->SetClassName(symbol_Handler);
        Nan::SetPrototypeMethod(constructor, "on", on);
        Nan::SetPrototypeMethod(constructor, "options", options);
        Nan::SetPrototypeMethod(constructor, "end", stream_end);
        target->Set(symbol_Handler, constructor->GetFunction());

        symbol_tagged_nodes_only = NODE_PSYMBOL("tagged_nodes_only");
    }

    JSHandler::JSHandler() :
        Nan::ObjectWrap(),
        node_callback_for_tagged_only(false) {
    }

    JSHandler::~JSHandler() {
        init_cb.Dispose();

        before_nodes_cb.Dispose();
        node_cb.Dispose();
        after_nodes_cb.Dispose();

        before_ways_cb.Dispose();
        way_cb.Dispose();
        after_ways_cb.Dispose();

        before_relations_cb.Dispose();
        relation_cb.Dispose();
        after_relations_cb.Dispose();

        area_cb.Dispose();

        before_changesets_cb.Dispose();
        changeset_cb.Dispose();
        after_changesets_cb.Dispose();

        done_cb.Dispose();
    }

    v8::Local<v8::Value> JSHandler::New(const v8::Arguments& info) {
        Nan::HandleScope scope;
        if (!info.IsConstructCall()) {
            ThrowException(v8::Exception::Error(Nan::New("Cannot call constructor as function, you need to use 'new' keyword").ToLocalChecked()));
            return;
        }
        if (info[0]->IsExternal()) {
            v8::Local<v8::External> ext = v8::Local<v8::External>::Cast(info[0]);
            void* ptr = ext->Value();
            JSHandler* b =  static_cast<JSHandler*>(ptr);
            b->Wrap(info.This());
            return info.This();
        } else {
            JSHandler* jshandler = new JSHandler();
            jshandler->Wrap(info.This());
            return info.This();
        }
        info.GetReturnValue().Set(Nan::Undefined());
        return;
    }

    v8::Local<v8::Value> JSHandler::options(const v8::Arguments& info) {
        INSTANCE_CHECK(JSHandler, "handler", "options");
        Nan::HandleScope scope;
        if (info.Length() != 1 || !info[0]->IsObject()) {
            ThrowException(v8::Exception::TypeError(Nan::New("please provide a single object as parameter").ToLocalChecked()));
            return;
        }

        v8::Local<v8::Value> tagged_nodes_only = info[0]->ToObject()->Get(symbol_tagged_nodes_only);
        if (tagged_nodes_only->IsBoolean()) {
            unwrap<JSHandler>(info.This()).node_callback_for_tagged_only = tagged_nodes_only->BooleanValue();
        }

        info.GetReturnValue().Set(Nan::Undefined());
        return;
    }

    v8::Local<v8::Value> JSHandler::on(const v8::Arguments& info) {
        INSTANCE_CHECK(JSHandler, "handler", "on");
        Nan::HandleScope scope;
        if (info.Length() != 2 || !info[0]->IsString() || !info[1]->IsFunction()) {
            ThrowException(v8::Exception::TypeError(Nan::New("please provide an event name and callback function").ToLocalChecked()));
            return;
        }
        v8::String::Utf8Value callback_name_string { info[0] };
        std::string callback_name = *callback_name_string;
        v8::Local<v8::Function> callback = v8::Local<v8::Function>::Cast(info[1]);
        if (callback->IsNull() || callback->IsUndefined()) {
            ThrowException(v8::Exception::TypeError(Nan::New("please provide a valid callback function for second arg").ToLocalChecked()));
            return;
        }

        JSHandler& handler = unwrap<JSHandler>(info.This());
        if (callback_name == "node") {
            handler.node_cb.Dispose();
            handler.node_cb = Nan::Persistent<v8::Function>::New(callback);
        } else if (callback_name == "way") {
            handler.way_cb.Dispose();
            handler.way_cb = Nan::Persistent<v8::Function>::New(callback);
        } else if (callback_name == "relation") {
            handler.relation_cb.Dispose();
            handler.relation_cb = Nan::Persistent<v8::Function>::New(callback);
        } else if (callback_name == "area") {
            handler.area_cb.Dispose();
            handler.area_cb = Nan::Persistent<v8::Function>::New(callback);
        } else if (callback_name == "changeset") {
            handler.changeset_cb.Dispose();
            handler.changeset_cb = Nan::Persistent<v8::Function>::New(callback);
        } else if (callback_name == "init") {
            handler.init_cb.Dispose();
            handler.init_cb = Nan::Persistent<v8::Function>::New(callback);
        } else if (callback_name == "before_nodes") {
            handler.before_nodes_cb.Dispose();
            handler.before_nodes_cb = Nan::Persistent<v8::Function>::New(callback);
        } else if (callback_name == "after_nodes") {
            handler.after_nodes_cb.Dispose();
            handler.after_nodes_cb = Nan::Persistent<v8::Function>::New(callback);
        } else if (callback_name == "before_ways") {
            handler.before_ways_cb.Dispose();
            handler.before_ways_cb = Nan::Persistent<v8::Function>::New(callback);
        } else if (callback_name == "after_ways") {
            handler.after_ways_cb.Dispose();
            handler.after_ways_cb = Nan::Persistent<v8::Function>::New(callback);
        } else if (callback_name == "before_relations") {
            handler.before_relations_cb.Dispose();
            handler.before_relations_cb = Nan::Persistent<v8::Function>::New(callback);
        } else if (callback_name == "after_relations") {
            handler.after_relations_cb.Dispose();
            handler.after_relations_cb = Nan::Persistent<v8::Function>::New(callback);
        } else if (callback_name == "before_changesets") {
            handler.before_changesets_cb.Dispose();
            handler.before_changesets_cb = Nan::Persistent<v8::Function>::New(callback);
        } else if (callback_name == "after_changesets") {
            handler.after_changesets_cb.Dispose();
            handler.after_changesets_cb = Nan::Persistent<v8::Function>::New(callback);
        } else if (callback_name == "done") {
            handler.done_cb.Dispose();
            handler.done_cb = Nan::Persistent<v8::Function>::New(callback);
        } else {
            ThrowException(v8::Exception::RangeError(Nan::New("unknown callback name as first argument").ToLocalChecked()));
            return;
        }

        info.GetReturnValue().Set(Nan::Undefined());
        return;
    }

    template <class TWrapped>
    void call_callback_with_entity(const Nan::Persistent<v8::Function>& function, const osmium::OSMEntity& entity) {
        if (function.IsEmpty()) {
            return;
        }

        Nan::HandleScope scope;
        v8::Local<v8::Value> argv[1] = { new_external<TWrapped>(entity) };
        function->Call(v8::Context::GetCurrent()->Global(), 1, argv);
    }

    void call_callback(const Nan::Persistent<v8::Function>& function) {
        if (function.IsEmpty()) {
            return;
        }

        function->Call(v8::Context::GetCurrent()->Global(), 0, nullptr);
    }

    void JSHandler::dispatch_entity(const osmium::OSMEntity& entity) const {
        switch (entity.type()) {
            case osmium::item_type::node:
                if (!node_cb.IsEmpty() && (!node_callback_for_tagged_only || !static_cast<const osmium::Node&>(entity).tags().empty())) {
                    call_callback_with_entity<OSMNodeWrap>(node_cb, entity);
                }
                break;
            case osmium::item_type::way:
                if (!way_cb.IsEmpty()) {
                    call_callback_with_entity<OSMWayWrap>(way_cb, entity);
                }
                break;
            case osmium::item_type::relation:
                if (!relation_cb.IsEmpty()) {
                    call_callback_with_entity<OSMRelationWrap>(relation_cb, entity);
                }
                break;
            case osmium::item_type::area:
                if (!area_cb.IsEmpty()) {
                    call_callback_with_entity<OSMAreaWrap>(area_cb, entity);
                }
                break;
            case osmium::item_type::changeset:
                if (!changeset_cb.IsEmpty()) {
                    call_callback_with_entity<OSMChangesetWrap>(changeset_cb, entity);
                }
                break;
            default:
                break;
        }
    }

    void JSHandler::init() const {
        call_callback(init_cb);
    }

    void JSHandler::before_nodes() const {
        call_callback(before_nodes_cb);
    }

    void JSHandler::after_nodes() const {
        call_callback(after_nodes_cb);
    }

    void JSHandler::before_ways() const {
        call_callback(before_ways_cb);
    }

    void JSHandler::after_ways() const {
        call_callback(after_ways_cb);
    }

    void JSHandler::before_relations() const {
        call_callback(before_relations_cb);
    }

    void JSHandler::after_relations() const {
        call_callback(after_relations_cb);
    }

    void JSHandler::before_changesets() const {
        call_callback(before_changesets_cb);
    }

    void JSHandler::after_changesets() const {
        call_callback(after_changesets_cb);
    }

    void JSHandler::done() const {
        call_callback(done_cb);
    }

    v8::Local<v8::Value> JSHandler::stream_end(const v8::Arguments& info) {
        INSTANCE_CHECK(JSHandler, "handler", "end");
        Nan::HandleScope scope;
        if (info.Length() != 0) {
            ThrowException(v8::Exception::TypeError(Nan::New("end() doesn't take any parameters").ToLocalChecked()));
            return;
        }

        JSHandler& handler = unwrap<JSHandler>(info.This());
        call_callback(handler.done_cb);

        info.GetReturnValue().Set(Nan::Undefined());
        return;
    }

} // namespace node_osmium

