
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

    v8::Persistent<v8::FunctionTemplate> BufferWrap::constructor;

    void BufferWrap::Initialize(v8::Handle<v8::Object> target) {
        v8::HandleScope scope;
        constructor = v8::Persistent<v8::FunctionTemplate>::New(v8::FunctionTemplate::New(BufferWrap::New));
        constructor->InstanceTemplate()->SetInternalFieldCount(1);
        constructor->SetClassName(symbol_Buffer);
        node::SetPrototypeMethod(constructor, "clear", clear);
        node::SetPrototypeMethod(constructor, "next", next);
        node::SetPrototypeMethod(constructor, "filter_point_in_time", filter_point_in_time);
        node::SetPrototypeMethod(constructor, "create_node_buffer", create_node_buffer);
        target->Set(symbol_Buffer, constructor->GetFunction());
    }

    v8::Handle<v8::Value> BufferWrap::New(const v8::Arguments& args) {
        if (args.Length() == 1 && args[0]->IsExternal()) {
            v8::Local<v8::External> ext = v8::Local<v8::External>::Cast(args[0]);
            static_cast<BufferWrap*>(ext->Value())->Wrap(args.This());
            return args.This();
        } else if (args.Length() == 1 && args[0]->IsObject()) {
            auto obj = args[0]->ToObject();
            if (node::Buffer::HasInstance(obj)) {
                try {
                    osmium::memory::Buffer buffer(reinterpret_cast<unsigned char*>(node::Buffer::Data(obj)), node::Buffer::Length(obj));
                    BufferWrap* buffer_wrap = new BufferWrap(std::move(buffer));
                    buffer_wrap->Wrap(args.This());

                    // Store the node::Buffer in the new osmium::Buffer object
                    // so the node::Buffer doesn't go away if it goes out of scope
                    // outside this function.
                    args.This()->Set(NODE_PSYMBOL("_data"), obj);

                    return args.This();
                } catch (std::exception const& ex) {
                    return ThrowException(v8::Exception::TypeError(v8::String::New(ex.what())));
                }
            }
        }
        return ThrowException(v8::Exception::TypeError(v8::String::New("osmium.Buffer takes a single argument, a node::Buffer")));
    }

    v8::Handle<v8::Value> BufferWrap::clear(const v8::Arguments& args) {
        INSTANCE_CHECK(BufferWrap, "Buffer", "clear");
        BufferWrap* buffer_wrap = node::ObjectWrap::Unwrap<BufferWrap>(args.This());
        buffer_wrap->m_this = std::move(osmium::memory::Buffer());
        buffer_wrap->m_iterator = buffer_wrap->m_this.end();
        return v8::Undefined();
    }

    v8::Handle<v8::Value> BufferWrap::next(const v8::Arguments& args) {
        INSTANCE_CHECK(BufferWrap, "Buffer", "next");
        BufferWrap* buffer_wrap = node::ObjectWrap::Unwrap<BufferWrap>(args.This());

        v8::HandleScope scope;

        int filter_id = 0;
        if (args.Length() == 1 && args[0]->IsInt32()) {
            filter_id = args[0]->ToInt32()->Value();
        }

        while (buffer_wrap->m_iterator != buffer_wrap->m_this.end()) {
            osmium::OSMEntity& entity = *buffer_wrap->m_iterator;
            ++buffer_wrap->m_iterator;

            if (Filter::get_filter(filter_id).match(entity)) {
                switch (entity.type()) {
                    case osmium::item_type::node: {
                        node::ObjectWrap::Unwrap<OSMNodeWrap>(the_Node)->set(entity);
                        return scope.Close(the_Node);
                    }
                    case osmium::item_type::way: {
                        node::ObjectWrap::Unwrap<OSMWayWrap>(the_Way)->set(entity);
                        return scope.Close(the_Way);
                    }
                    case osmium::item_type::relation: {
                        node::ObjectWrap::Unwrap<OSMRelationWrap>(the_Relation)->set(entity);
                        return scope.Close(the_Relation);
                    }
                    case osmium::item_type::area: {
                        node::ObjectWrap::Unwrap<OSMAreaWrap>(the_Area)->set(entity);
                        return scope.Close(the_Area);
                    }
                    case osmium::item_type::changeset: {
                        node::ObjectWrap::Unwrap<OSMChangesetWrap>(the_Changeset)->set(entity);
                        return scope.Close(the_Changeset);
                    }
                    default:
                        assert(false);
                        break;
                }
            }
        }

        return scope.Close(v8::Undefined());
    }

    v8::Handle<v8::Value> BufferWrap::filter_point_in_time(const v8::Arguments& args) {
        INSTANCE_CHECK(BufferWrap, "Buffer", "filter_point_in_time");
        v8::HandleScope scope;
        if (args.Length() != 1) {
            return ThrowException(v8::Exception::TypeError(v8::String::New("please provide a point in time as first and only argument")));
        }

        osmium::Timestamp point_in_time;
        if (args[0]->IsInt32()) {
            point_in_time = args[0]->Int32Value();
        } else if (args[0]->IsString()) {
            v8::String::Utf8Value time_string { args[0] };
            point_in_time = osmium::Timestamp(*time_string);
        } else if (args[0]->IsDate()) {
            point_in_time = osmium::Timestamp(static_cast<int32_t>(v8::Date::Cast(*args[0])->NumberValue() / 1000));
        }

        typedef osmium::DiffIterator<osmium::memory::Buffer::t_iterator<osmium::OSMObject>> diff_iterator;
        osmium::memory::Buffer& buffer = unwrap<BufferWrap>(args.This());
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

        return scope.Close(new_external<BufferWrap>(std::move(fbuffer)));
    }

    v8::Handle<v8::Value> BufferWrap::create_node_buffer(const v8::Arguments& args) {
        INSTANCE_CHECK(BufferWrap, "Buffer", "create_node_buffer");
        osmium::memory::Buffer& buffer = unwrap<BufferWrap>(args.This());

        if (!buffer) {
            return v8::Undefined();
        }

        v8::HandleScope scope;

        int length = buffer.committed();
        node::Buffer* slow_buffer = node::Buffer::New(length);
        std::copy_n(buffer.data(), length, node::Buffer::Data(slow_buffer));

        v8::Local<v8::Object> global = v8::Context::GetCurrent()->Global();

        v8::Local<v8::Function> buffer_constructor =
            v8::Local<v8::Function>::Cast(global->Get(v8::String::New("Buffer")));

        v8::Handle<v8::Value> constructor_args[3] = {
            slow_buffer->handle_,
            v8::Integer::New(length),
            v8::Integer::New(0)
        };

        return scope.Close(buffer_constructor->NewInstance(3, constructor_args));
    }


} // namespace node_osmium

