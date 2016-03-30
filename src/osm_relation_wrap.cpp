
// node-osmium
#include "osm_relation_wrap.hpp"

namespace node_osmium {

    Nan::Persistent<v8::FunctionTemplate> OSMRelationWrap::constructor;

    void OSMRelationWrap::Initialize(v8::Local<v8::Object> target) {
        Nan::HandleScope scope;
        v8::Local<v8::FunctionTemplate> lcons = Nan::New<v8::FunctionTemplate>(OSMRelationWrap::New);
        lcons->Inherit(Nan::New(OSMWrappedObject::constructor));
        lcons->InstanceTemplate()->SetInternalFieldCount(1);
        lcons->SetClassName(Nan::New(symbol_Relation));
        ATTR(lcons, "type", get_type);
        ATTR(lcons, "members_count", get_members_count);
        Nan::SetPrototypeMethod(lcons, "members", members);
        target->Set(Nan::New(symbol_Relation), lcons->GetFunction());
        constructor.Reset(lcons);
    }

    NAN_METHOD(OSMRelationWrap::New) {
        if (info.Length() == 1 && info[0]->IsExternal()) {
            v8::Local<v8::External> ext = v8::Local<v8::External>::Cast(info[0]);
            static_cast<OSMRelationWrap*>(ext->Value())->Wrap(info.This());
            info.GetReturnValue().Set(info.This());
            return;
        } else {
            Nan::ThrowTypeError(Nan::New("osmium.Relation cannot be created in Javascript").ToLocalChecked());
            return;
        }
    }

    NAN_GETTER(OSMRelationWrap::get_members_count) {
        info.GetReturnValue().Set(Nan::New<v8::Number>(wrapped(info.This()).members().size()));
        return;
    }

    NAN_METHOD(OSMRelationWrap::members) {
        const osmium::Relation& relation = wrapped(info.This());

        switch (info.Length()) {
            case 0: {
                v8::Local<v8::Array> members = Nan::New<v8::Array>();
                int i = 0;
                char typec[2] = " ";
                for (const auto& member : relation.members()) {
                    v8::Local<v8::Object> jsmember = Nan::New<v8::Object>();
                    typec[0] = osmium::item_type_to_char(member.type());
                    jsmember->Set(Nan::New(symbol_type), Nan::New(typec).ToLocalChecked());
                    jsmember->Set(Nan::New(symbol_ref), Nan::New<v8::Number>(member.ref()));
                    jsmember->Set(Nan::New(symbol_role), Nan::New(member.role()).ToLocalChecked());
                    members->Set(i, jsmember);
                    ++i;
                }
                info.GetReturnValue().Set(members);
                return;
            }
            case 1: {
                if (!info[0]->IsUint32()) {
                    Nan::ThrowTypeError(Nan::New("call members() without parameters or the index of the member you want").ToLocalChecked());
                    return;
                }
                uint32_t n = info[0]->ToUint32()->Value();
                if (n < relation.members().size()) {
                    auto it = relation.members().begin();
                    std::advance(it, n);
                    const osmium::RelationMember& member = *it;
                    v8::Local<v8::Object> jsmember = Nan::New<v8::Object>();
                    char typec[2] = " ";
                    typec[0] = osmium::item_type_to_char(member.type());
                    jsmember->Set(Nan::New(symbol_type), Nan::New(typec).ToLocalChecked());
                    jsmember->Set(Nan::New(symbol_ref), Nan::New<v8::Number>(member.ref()));
                    jsmember->Set(Nan::New(symbol_role), Nan::New(member.role()).ToLocalChecked());
                    info.GetReturnValue().Set(jsmember);
                    return;
                } else {
                    Nan::ThrowRangeError(Nan::New("argument to members() out of range").ToLocalChecked());
                    return;
                }
            }
        }

        Nan::ThrowTypeError(Nan::New("call members() without parameters or the index of the member you want").ToLocalChecked());
        return;
    }

} // namespace node_osmium

