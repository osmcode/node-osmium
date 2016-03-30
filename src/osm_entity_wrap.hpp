#ifndef OSM_ENTITY_WRAP_HPP
#define OSM_ENTITY_WRAP_HPP

#include "include_nan.hpp"

// osmium
namespace osmium {
    class OSMEntity;
}

#include "utils.hpp"

namespace node_osmium {

    class OSMEntityWrap : public Nan::ObjectWrap {

        const osmium::OSMEntity* m_entity;

    protected:

        typedef v8::Handle<v8::Value> accessor_type(v8::Local<v8::String> property, const v8::AccessorInfo& info);

        static void set_accessor(v8::Persistent<v8::FunctionTemplate> t, const char* name, accessor_type getter, v8::PropertyAttribute attributes) {
            t->InstanceTemplate()->SetAccessor(v8::String::NewSymbol(name), getter, nullptr, v8::Handle<v8::Value>(), v8::DEFAULT, attributes);
        }

        template<class T>
        static NAN_METHOD(tags_impl) {
            Nan::HandleScope scope;

            const T& object = static_cast<const T&>(unwrap<OSMEntityWrap>(info.This()));
            switch (info.Length()) {
                case 0: {
                    v8::Local<v8::Object> tags = Nan::New<v8::Object>();
                    for (const auto& tag : object.tags()) {
                        tags->Set(Nan::New(tag.key()), v8::String::New(tag.value()));
                    }
                    info.GetReturnValue().Set(tags);
                    return;
                }
                case 1: {
                    if (!info[0]->IsString()) {
                        break;
                    }
                    v8::String::Utf8Value key { info[0] };
                    const char* value = object.tags().get_value_by_key(*key);
                    info.GetReturnValue().Set((value ? Nan::New(value) : Nan::Undefined()).ToLocalChecked());
                    return;
                }
            }

            ThrowException(v8::Exception::TypeError(Nan::New("call tags() without parameters or with a string (the key)").ToLocalChecked()));
            return;
        }

    public:

        static Nan::Persistent<v8::FunctionTemplate> constructor;
        static void Initialize(v8::Local<v8::Object> target);
        static NAN_METHOD(New);

        OSMEntityWrap() :
            m_entity(nullptr) {
        }

        OSMEntityWrap(const osmium::OSMEntity& entity) :
            m_entity(&entity) {
        }

        void set(const osmium::OSMEntity& entity) {
            m_entity = &entity;
        }

        const osmium::OSMEntity& get() {
            return *m_entity;
        }

    protected:

        virtual ~OSMEntityWrap() = default;

    }; // class OSMEntityWrap

} // namespace node_osmium

#endif // OSM_ENTITY_WRAP_HPP
