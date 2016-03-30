
#include <algorithm>

// osmium
#include <osmium/diff_iterator.hpp>

// node-osmium
#include "filter.hpp"
#include "node_osmium.hpp"
#include "buffer_wrap.hpp"
#include "osm_node_wrap.hpp"
#include "osm_way_wrap.hpp"
#include "osm_relation_wrap.hpp"
#include "osm_area_wrap.hpp"
#include "osm_changeset_wrap.hpp"
#include "utils.hpp"

namespace node_osmium {

    Nan::Persistent<v8::FunctionTemplate> BufferWrap::constructor;

    void BufferWrap::Initialize(v8::Local<v8::Object> target) {
        Nan::HandleScope scope;
        v8::Local<v8::FunctionTemplate> lcons = Nan::New<v8::FunctionTemplate>(BufferWrap::New);
        lcons->InstanceTemplate()->SetInternalFieldCount(1);
        lcons->SetClassName(Nan::New(symbol_Buffer));
        Nan::SetPrototypeMethod(lcons, "clear", clear);
        Nan::SetPrototypeMethod(lcons, "next", next);
        Nan::SetPrototypeMethod(lcons, "filter_point_in_time", filter_point_in_time);
        Nan::SetPrototypeMethod(lcons, "create_node_buffer", create_node_buffer);
        target->Set(Nan::New(symbol_Buffer), lcons->GetFunction());
        constructor.Reset(lcons);
    }

    NAN_METHOD(BufferWrap::New) {
        if (info.Length() == 1 && info[0]->IsExternal()) {
            v8::Local<v8::External> ext = v8::Local<v8::External>::Cast(info[0]);
            static_cast<BufferWrap*>(ext->Value())->Wrap(info.This());
            info.GetReturnValue().Set(info.This());
            return;
        } else if (info.Length() == 1 && info[0]->IsObject()) {
            auto obj = info[0]->ToObject();
            if (node::Buffer::HasInstance(obj)) {
                try {
                    osmium::memory::Buffer buffer(reinterpret_cast<unsigned char*>(node::Buffer::Data(obj)), node::Buffer::Length(obj));
                    BufferWrap* buffer_wrap = new BufferWrap(std::move(buffer));
                    buffer_wrap->Wrap(info.This());

                    // Store the node::Buffer in the new osmium::Buffer object
                    // so the node::Buffer doesn't go away if it goes out of scope
                    // outside this function.
                    info.This()->Set(NODE_PSYMBOL("_data"), obj);
                    info.GetReturnValue().Set(info.This());
                    return;
                } catch (std::exception const& ex) {
                    ThrowException(v8::Exception::TypeError(Nan::New(ex.what()).ToLocalChecked()));
                    return;
                }
            }
        }
        ThrowException(v8::Exception::TypeError(Nan::New("osmium.Buffer takes a single argument, a node::Buffer").ToLocalChecked()));
        return;
    }

    NAN_METHOD(BufferWrap::clear) {
        INSTANCE_CHECK(BufferWrap, "Buffer", "clear");
        BufferWrap* buffer_wrap = Nan::ObjectWrap::Unwrap<BufferWrap>(info.This());
        buffer_wrap->m_this = std::move(osmium::memory::Buffer());
        buffer_wrap->m_iterator = buffer_wrap->m_this.end();
        info.GetReturnValue().Set(Nan::Undefined());
    }

    NAN_METHOD(BufferWrap::next) {
        INSTANCE_CHECK(BufferWrap, "Buffer", "next");
        BufferWrap* buffer_wrap = Nan::ObjectWrap::Unwrap<BufferWrap>(info.This());

        Nan::HandleScope scope;

        int filter_id = 0;
        if (info.Length() == 1 && info[0]->IsInt32()) {
            filter_id = info[0]->ToInt32()->Value();
        }

        while (buffer_wrap->m_iterator != buffer_wrap->m_this.end()) {
            osmium::OSMEntity& entity = *buffer_wrap->m_iterator;
            ++buffer_wrap->m_iterator;

            if (Filter::get_filter(filter_id).match(entity)) {
                switch (entity.type()) {
                    case osmium::item_type::node: {
                        Nan::ObjectWrap::Unwrap<OSMNodeWrap>(Nan::New(the_Node))->set(entity);
                        info.GetReturnValue().Set(Nan::New(the_Node));
                        return;
                    }
                    case osmium::item_type::way: {
                        Nan::ObjectWrap::Unwrap<OSMWayWrap>(Nan::New(the_Way))->set(entity);
                        info.GetReturnValue().Set(Nan::New(the_Way));
                        return;
                    }
                    case osmium::item_type::relation: {
                        Nan::ObjectWrap::Unwrap<OSMRelationWrap>(Nan::New(the_Relation))->set(entity);
                        info.GetReturnValue().Set(Nan::New(the_Relation));
                        return;
                    }
                    case osmium::item_type::area: {
                        Nan::ObjectWrap::Unwrap<OSMAreaWrap>(Nan::New(the_Area))->set(entity);
                        info.GetReturnValue().Set(Nan::New(the_Area));
                        return;
                    }
                    case osmium::item_type::changeset: {
                        Nan::ObjectWrap::Unwrap<OSMChangesetWrap>(Nan::New(the_Changeset))->set(entity);
                        info.GetReturnValue().Set(Nan::New(the_Changeset));
                        return;
                    }
                    default:
                        assert(false);
                        break;
                }
            }
        }

        info.GetReturnValue().Set(Nan::Undefined());
        return;
    }

    NAN_METHOD(BufferWrap::filter_point_in_time) {
        INSTANCE_CHECK(BufferWrap, "Buffer", "filter_point_in_time");
        Nan::HandleScope scope;
        if (info.Length() != 1) {
            ThrowException(v8::Exception::TypeError(Nan::New("please provide a point in time as first and only argument").ToLocalChecked()));
            return;
        }

        osmium::Timestamp point_in_time;
        if (info[0]->IsInt32()) {
            point_in_time = info[0]->Int32Value();
        } else if (info[0]->IsString()) {
            v8::String::Utf8Value time_string { info[0] };
            point_in_time = osmium::Timestamp(*time_string);
        } else if (info[0]->IsDate()) {
            point_in_time = osmium::Timestamp(static_cast<int32_t>(v8::Date::Cast(*info[0])->NumberValue() / 1000));
        }

        typedef osmium::DiffIterator<osmium::memory::Buffer::t_iterator<osmium::OSMObject>> diff_iterator;
        osmium::memory::Buffer& buffer = unwrap<BufferWrap>(info.This());
        osmium::memory::Buffer fbuffer(buffer.committed(), osmium::memory::Buffer::auto_grow::yes);
        {
            auto dbegin = diff_iterator(buffer.begin<osmium::OSMObject>(), buffer.end<osmium::OSMObject>());
            auto dend   = diff_iterator(buffer.end<osmium::OSMObject>(), buffer.end<osmium::OSMObject>());

            std::for_each(dbegin, dend, [point_in_time, &fbuffer](const osmium::DiffObject& d) {
                if (d.is_visible_at(point_in_time)) {
                    fbuffer.add_item(d.curr());
                    fbuffer.commit();
                }
            });
        }

        info.GetReturnValue().Set(new_external<BufferWrap>(std::move(fbuffer)));
        return;
    }

    NAN_METHOD(BufferWrap::create_node_buffer) {
        INSTANCE_CHECK(BufferWrap, "Buffer", "create_node_buffer");
        osmium::memory::Buffer& buffer = unwrap<BufferWrap>(info.This());

        if (!buffer) {
            info.GetReturnValue().Set(Nan::Undefined());
            return;
        }

        Nan::HandleScope scope;

        int length = buffer.committed();
        v8::Local<v8::Object> slow_buffer = Nan::NewBuffer(length).ToLocalChecked();
        std::copy_n(buffer.data(), length, node::Buffer::Data(slow_buffer));

        v8::Local<v8::Object> global = v8::Context::GetCurrent()->Global();

        v8::Local<v8::Function> buffer_constructor =
            v8::Local<v8::Function>::Cast(global->Get(Nan::New("Buffer").ToLocalChecked()));

        v8::Local<v8::Value> constructor_info[3] = {
            slow_buffer,
            Nan::New(length),
            Nan::New(0)
        };

        info.GetReturnValue().Set(buffer_constructor->NewInstance(3, constructor_info));
        return;
    }


} // namespace node_osmium

