
// node-osmium
#include "osm_relation_wrap.hpp"

namespace node_osmium {

    Nan::Persistent<v8::FunctionTemplate> OSMRelationWrap::constructor;

    void OSMRelationWrap::Initialize(v8::Local<v8::Object> target) {
        Nan::HandleScope scope;
        v8::Local<v8::FunctionTemplate> lcons = Nan::New<v8::FunctionTemplate>(OSMRelationWrap::New);
        constructor->Inherit(OSMWrappedObject::constructor);
        lcons->InstanceTemplate()->SetInternalFieldCount(1);
        lcons->SetClassName(Nan::New(symbol_Relation));
        auto attributes = static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete);
        set_accessor(constructor, "type", get_type, attributes);
        set_accessor(constructor, "members_count", get_members_count, attributes);
        Nan::SetPrototypeMethod(lcons, "members", members);
        target->Set(Nan::New(symbol_Relation), lcons->GetFunction());
    }

    NAN_METHOD(OSMRelationWrap::New) {
        if (info.Length() == 1 && info[0]->IsExternal()) {
            v8::Local<v8::External> ext = v8::Local<v8::External>::Cast(info[0]);
            static_cast<OSMRelationWrap*>(ext->Value())->Wrap(info.This());
            info.GetReturnValue().Set(info.This());
            return;
        } else {
            ThrowException(v8::Exception::TypeError(Nan::New("osmium.Relation cannot be created in Javascript").ToLocalChecked()));
            return;
        }
    }

    NAN_METHOD(OSMRelationWrap::get_members_count) {
        Nan::HandleScope scope;
        info.GetReturnValue().Set(Nan::New(wrapped(info.This()).members().size()));
        return;
    }

    NAN_METHOD(OSMRelationWrap::members) {
        Nan::HandleScope scope;

        const osmium::Relation& relation = wrapped(info.This());

        switch (info.Length()) {
            case 0: {
                v8::Local<v8::Array> members = Nan::New();
                int i = 0;
                char typec[2] = " ";
                for (const auto& member : relation.members()) {
                    v8::Local<v8::Object> jsmember = Nan::New();
                    typec[0] = osmium::item_type_to_char(member.type());
                    jsmember->Set(symbol_type, Nan::New(typec));
                    jsmember->Set(symbol_ref, Nan::New(member.ref()));
                    jsmember->Set(symbol_role, Nan::New(member.role()));
                    members->Set(i, jsmember);
                    ++i;
                }
                info.GetReturnValue().Set(members);
                return;
            }
            case 1: {
                if (!info[0]->IsUint32()) {
                    ThrowException(v8::Exception::TypeError(Nan::New("call members() without parameters or the index of the member you want").ToLocalChecked()));
                    return;
                }
                uint32_t n = info[0]->ToUint32()->Value();
                if (n < relation.members().size()) {
                    auto it = relation.members().begin();
                    std::advance(it, n);
                    const osmium::RelationMember& member = *it;
                    v8::Local<v8::Object> jsmember = Nan::New();
                    char typec[2] = " ";
                    typec[0] = osmium::item_type_to_char(member.type());
                    jsmember->Set(symbol_type, Nan::New(typec));
                    jsmember->Set(symbol_ref, Nan::New(member.ref()));
                    jsmember->Set(symbol_role, Nan::New(member.role()));
                    info.GetReturnValue().Set(jsmember);
                    return;
                } else {
                    ThrowException(v8::Exception::RangeError(Nan::New("argument to members() out of range").ToLocalChecked()));
                    return;
                }
            }
        }

        ThrowException(v8::Exception::TypeError(Nan::New("call members() without parameters or the index of the member you want").ToLocalChecked()));
        return;
    }

} // namespace node_osmium

