#ifndef HANDLER_HPP
#define HANDLER_HPP

#include "include_nan.hpp"

// osmium
namespace osmium {
    class OSMEntity;
}

namespace node_osmium {

    class JSHandler : public Nan::ObjectWrap {

        static Nan::Persistent<v8::String> symbol_tagged_nodes_only;

        bool node_callback_for_tagged_only;

        Nan::Persistent<v8::Function> init_cb;

        Nan::Persistent<v8::Function> before_nodes_cb;
        Nan::Persistent<v8::Function> node_cb;
        Nan::Persistent<v8::Function> after_nodes_cb;

        Nan::Persistent<v8::Function> before_ways_cb;
        Nan::Persistent<v8::Function> way_cb;
        Nan::Persistent<v8::Function> after_ways_cb;

        Nan::Persistent<v8::Function> before_relations_cb;
        Nan::Persistent<v8::Function> relation_cb;
        Nan::Persistent<v8::Function> after_relations_cb;

        Nan::Persistent<v8::Function> area_cb;

        Nan::Persistent<v8::Function> before_changesets_cb;
        Nan::Persistent<v8::Function> changeset_cb;
        Nan::Persistent<v8::Function> after_changesets_cb;

        Nan::Persistent<v8::Function> done_cb;

        static NAN_METHOD(on);
        static NAN_METHOD(options);
        static NAN_METHOD(stream_end);

    public:

        static Nan::Persistent<v8::FunctionTemplate> constructor;
        static void Initialize(v8::Local<v8::Object> target);
        static NAN_METHOD(New);

        JSHandler();

        JSHandler& get() {
            return *this;
        }

        void dispatch_entity(const osmium::OSMEntity& entity) const;

        void init() const;
        void before_nodes() const;
        void after_nodes() const;
        void before_ways() const;
        void after_ways() const;
        void before_relations() const;
        void after_relations() const;
        void before_changesets() const;
        void after_changesets() const;
        void done() const;

    private:

        ~JSHandler();

    }; // class JSHandler

} // namespace node_osmium

#endif // HANDLER_HPP
