#ifndef OSM_ENTITY_WRAP_HPP
#define OSM_ENTITY_WRAP_HPP

#include "include_nan.hpp"

// osmium
namespace osmium {
    class OSMEntity;
}

#include "utils.hpp"

#define ATTR(t, name, get)                                         \
    Nan::SetAccessor(t->InstanceTemplate(), Nan::New<v8::String>(name).ToLocalChecked(), get, nullptr);

namespace node_osmium {

    class OSMEntityWrap : public Nan::ObjectWrap {

        const osmium::OSMEntity* m_entity;

    protected:

        template<class T>
        static NAN_METHOD(tags_impl) {
            Nan::HandleScope scope;

            const T& object = static_cast<const T&>(unwrap<OSMEntityWrap>(info.This()));
            switch (info.Length()) {
                case 0: {
                    v8::Local<v8::Object> tags = Nan::New<v8::Object>();
                    for (const auto& tag : object.tags()) {
                        Nan::Set(tags, Nan::New(tag.key()).ToLocalChecked(), Nan::New(tag.value()).ToLocalChecked());
                    }
                    info.GetReturnValue().Set(tags);
                    return;
                }
                case 1: {
                    if (!info[0]->IsString()) {
                        break;
                    }
                    Nan::Utf8String key { info[0] };
                    const char* value = object.tags().get_value_by_key(*key);
                    if (value) {
                        info.GetReturnValue().Set(Nan::New(value).ToLocalChecked());
                        return;
                    }
                    info.GetReturnValue().Set(Nan::Undefined());
                    return;
                }
            }

            Nan::ThrowTypeError(Nan::New("call tags() without parameters or with a string (the key)").ToLocalChecked());
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
