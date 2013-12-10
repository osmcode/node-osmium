// c++11
#include <iterator>

// v8
#include <v8.h>

// node
#include <node.h>
#include <node_version.h>
#include <node_object_wrap.h>

// osmium
#include <osmium/osm/relation.hpp>
#include <osmium/io/input_iterator.hpp>
#include <osmium/io/reader.hpp>

#include "osm_object.hpp"

using namespace v8;

namespace node_osmium {

    class Relation : public OSMObject {

    public:

        static Persistent<FunctionTemplate> constructor;
        static void Initialize(Handle<Object> target);
        static Handle<Value> New(const Arguments& args);
        static Handle<Value> members(const Arguments& args);
        Relation(const input_iterator&);

        void _ref() {
            Ref();
        }

        void _unref() {
            Unref();
        }

        osmium::Relation& object() {
            return static_cast<osmium::Relation&>(*m_it);
        }

    private:

        ~Relation();

    };

    Persistent<FunctionTemplate> Relation::constructor;

    void Relation::Initialize(Handle<Object> target) {
        HandleScope scope;
        constructor = Persistent<FunctionTemplate>::New(FunctionTemplate::New(Relation::New));
        constructor->InstanceTemplate()->SetInternalFieldCount(1);
        constructor->SetClassName(String::NewSymbol("Relation"));
        NODE_SET_PROTOTYPE_METHOD(constructor, "tags", tags);
        NODE_SET_PROTOTYPE_METHOD(constructor, "members", members);
        target->Set(String::NewSymbol("Relation"), constructor->GetFunction());
    }

    Relation::Relation(const input_iterator& it) :
        OSMObject(it) {
    }

    Relation::~Relation() {
    }

    Handle<Value> Relation::New(const Arguments& args) {
        HandleScope scope;
        if (args[0]->IsExternal()) {
            Local<External> ext = Local<External>::Cast(args[0]);
            void* ptr = ext->Value();
            Relation* relation = static_cast<Relation*>(ptr);
            relation->Wrap(args.This());
            osmium::Relation& obj = static_cast<osmium::Relation&>(*(relation->m_it));
            args.This()->Set(String::NewSymbol("id"), Number::New(obj.id()));
            args.This()->Set(String::NewSymbol("version"), Number::New(obj.version()));
            args.This()->Set(String::NewSymbol("changeset"), Number::New(obj.changeset()));
            args.This()->Set(String::NewSymbol("visible"), Boolean::New(obj.visible()));
            args.This()->Set(String::NewSymbol("timestamp"), Number::New(obj.timestamp()));
            args.This()->Set(String::NewSymbol("timestamp_iso"), String::New(obj.timestamp().to_iso().c_str(), obj.timestamp().to_iso().size()));
            args.This()->Set(String::NewSymbol("uid"), Number::New(obj.uid()));
            args.This()->Set(String::NewSymbol("user"), String::New(obj.user()));
            return args.This();
        } else {
            return ThrowException(Exception::TypeError(String::New("osmium.Relation cannot be created in Javascript")));
        }
        return Undefined();
    }

    Handle<Value> Relation::members(const Arguments& args) {
        HandleScope scope;
        osmium::Relation& relation = static_cast<osmium::Relation&>(*(node::ObjectWrap::Unwrap<Relation>(args.This())->m_it));

        if (args.Length() == 0) {
            Local<Array> members = Array::New();
            int i = 0;
            char typec[2] = " ";
            for (auto& member : relation.members()) {
                Local<Object> jsmember = Object::New();
                typec[0] = osmium::item_type_to_char(member.type());
                jsmember->Set(String::NewSymbol("type"), String::New(typec));
                jsmember->Set(String::NewSymbol("ref"), Number::New(member.ref()));
                jsmember->Set(String::NewSymbol("role"), String::New(member.role()));
                members->Set(i, jsmember);
                ++i;
            }
            return scope.Close(members);
        } else if (args.Length() == 1) {
            if (args[0]->IsNumber()) {
                int n = static_cast<int>(args[0]->ToNumber()->Value());
                if (n > 0 && n < static_cast<int>(relation.members().size())) {
                    auto it = relation.members().begin();
                    std::advance(it, n);
                    osmium::RelationMember& member = *it;
                    Local<Object> jsmember = Object::New();
                    char typec[2] = " ";
                    typec[0] = osmium::item_type_to_char(member.type());
                    jsmember->Set(String::NewSymbol("type"), String::New(typec));
                    jsmember->Set(String::NewSymbol("ref"), Number::New(member.ref()));
                    jsmember->Set(String::NewSymbol("role"), String::New(member.role()));
                    return scope.Close(jsmember);
                }
            }
        }

        return Undefined();
    }

} // namespace node_osmium