
// boost
#include <boost/variant.hpp>

// nan
#include <nan.h>

// osmium
#include <osmium/io/input_iterator.hpp>
#include <osmium/memory/buffer.hpp>
#include <osmium/visitor.hpp>

// node-osmium
#include "apply.hpp"
#include "buffer_wrap.hpp"
#include "handler.hpp"
#include "location_handler_wrap.hpp"
#include "multipolygon_collector_wrap.hpp"
#include "multipolygon_handler_wrap.hpp"
#include "basic_reader_wrap.hpp"
#include "flex_reader_wrap.hpp"
#include "utils.hpp"

namespace node_osmium {

    struct visitor_type : public boost::static_visitor<> {

        osmium::OSMEntity& m_entity;

        visitor_type(osmium::OSMEntity& entity) :
            m_entity(entity) {
        }

        void operator()(JSHandler& handler) const {
            handler.dispatch_entity(m_entity);
        }

        void operator()(location_handler_type& handler) const {
            osmium::apply_item(m_entity, handler);
        }

        void operator()(osmium::area::MultipolygonCollector<osmium::area::Assembler>::HandlerPass2& handler) const {
            osmium::apply_item(m_entity, handler);
        }

    }; // struct visitor_type

    struct visitor_before_after_type : public boost::static_visitor<> {

        osmium::item_type m_last;
        osmium::item_type m_current;

        visitor_before_after_type(osmium::item_type last, osmium::item_type current) :
            m_last(last),
            m_current(current) {
        }

        // The operator() is overloaded just for location_handler and
        // for JSHandler. Currently these are the only handlers allowed
        // anyways. But this needs to be fixed at some point to allow
        // any handler. Unfortunately a template function is not allowed
        // here.
        void operator()(location_handler_type& visitor) const {
        }

        void operator()(osmium::area::MultipolygonCollector<osmium::area::Assembler>::HandlerPass2& handler) const {
        }

        void operator()(JSHandler& visitor) const {
            switch (m_last) {
                case osmium::item_type::undefined:
                    visitor.init();
                    break;
                case osmium::item_type::node:
                    visitor.after_nodes();
                    break;
                case osmium::item_type::way:
                    visitor.after_ways();
                    break;
                case osmium::item_type::relation:
                    visitor.after_relations();
                    break;
                case osmium::item_type::changeset:
                    visitor.after_changesets();
                    break;
                default:
                    break;
            }
            switch (m_current) {
                case osmium::item_type::node:
                    visitor.before_nodes();
                    break;
                case osmium::item_type::way:
                    visitor.before_ways();
                    break;
                case osmium::item_type::relation:
                    visitor.before_relations();
                    break;
                case osmium::item_type::changeset:
                    visitor.before_changesets();
                    break;
                default:
                    break;
            }
        }

    }; // struct visitor_before_after_type

    typedef boost::variant<location_handler_type&, JSHandler&, osmium::area::MultipolygonCollector<osmium::area::Assembler>::HandlerPass2&> some_handler_type;

    template <class TIter>
    v8::Local<v8::Value> apply_iterator(TIter it, TIter end, std::vector<some_handler_type>& handlers) {
        struct javascript_error {};

        Nan::EscapableHandleScope scope;
        Nan::TryCatch trycatch;
        try {
            osmium::item_type last_type = osmium::item_type::undefined;

            for (; it != end; ++it) {
                visitor_before_after_type visitor_before_after(last_type, it->type());
                visitor_type visitor(*it);

                for (auto& handler : handlers) {
                    if (last_type != it->type()) {
                        boost::apply_visitor(visitor_before_after, handler);
                        if (trycatch.HasCaught()) {
                            throw javascript_error();
                        }
                    }
                    boost::apply_visitor(visitor, handler);
                    if (trycatch.HasCaught()) {
                        throw javascript_error();
                    }
                }

                if (last_type != it->type()) {
                    last_type = it->type();
                }
            }

            visitor_before_after_type visitor_before_after(last_type, osmium::item_type::undefined);
            for (auto& handler : handlers) {
                boost::apply_visitor(visitor_before_after, handler);
                if (trycatch.HasCaught()) {
                    throw javascript_error();
                }
            }
        } catch (const javascript_error&) {
            trycatch.ReThrow();
        }
        return scope.Escape(Nan::Undefined());
    }

    NAN_METHOD(apply) {
        if (info.Length() > 0 && info[0]->IsObject()) {
            std::vector<some_handler_type> handlers;

            for (int i=1; i != info.Length(); ++i) {
                if (!info[i]->IsObject()) {
                    Nan::ThrowTypeError(Nan::New("please provide handler objects as second and further parameters to apply()").ToLocalChecked());
                    return;
                }
                auto obj = info[i]->ToObject();
                if (Nan::New(JSHandler::constructor)->HasInstance(obj)) {
                    handlers.push_back(unwrap<JSHandler>(obj));
                } else if (Nan::New(LocationHandlerWrap::constructor)->HasInstance(obj)) {
                    handlers.push_back(unwrap<LocationHandlerWrap>(obj));
                } else if (Nan::New(MultipolygonHandlerWrap::constructor)->HasInstance(obj)) {
                    handlers.push_back(unwrap<MultipolygonHandlerWrap>(obj));
                }
            }

            try {
                auto source = info[0]->ToObject();
                if (Nan::New(BasicReaderWrap::constructor)->HasInstance(source)) {
                    osmium::io::Reader& reader = unwrap<BasicReaderWrap>(source);

                    if (reader.eof()) {
                        Nan::ThrowError(Nan::New("apply() called on a reader that has reached EOF").ToLocalChecked());
                        return;
                    }

                    typedef osmium::io::InputIterator<osmium::io::Reader, osmium::OSMEntity> input_iterator;

                    info.GetReturnValue().Set(apply_iterator(input_iterator{reader}, input_iterator{}, handlers));
                    return;
                } else if (Nan::New(FlexReaderWrap::constructor)->HasInstance(source)) {
                    flex_reader_type& reader = unwrap<FlexReaderWrap>(source);

                    if (reader.eof()) {
                        Nan::ThrowError(Nan::New("apply() called on a reader that has reached EOF").ToLocalChecked());
                        return;
                    }

                    typedef osmium::io::InputIterator<flex_reader_type, osmium::OSMEntity> input_iterator;

                    info.GetReturnValue().Set(apply_iterator(input_iterator{reader}, input_iterator{}, handlers));
                    return;
                } else if (Nan::New(BufferWrap::constructor)->HasInstance(source)) {
                    osmium::memory::Buffer& buffer = unwrap<BufferWrap>(source);
                    info.GetReturnValue().Set(apply_iterator(buffer.begin(), buffer.end(), handlers));
                    return;
                } else if (node::Buffer::HasInstance(source)) {
                    osmium::memory::Buffer buffer(reinterpret_cast<unsigned char*>(node::Buffer::Data(source)), node::Buffer::Length(source));

                    info.GetReturnValue().Set(apply_iterator(buffer.begin<osmium::OSMEntity>(), buffer.end<osmium::OSMEntity>(), handlers));
                    return;
                }
            } catch (const std::exception& e) {
                std::string msg("osmium error: ");
                msg += e.what();
                Nan::ThrowError(Nan::New(msg).ToLocalChecked());
                return;
            }
        }

        Nan::ThrowTypeError(Nan::New("please provide a BasicReader, FlexReader or Buffer object as first parameter").ToLocalChecked());
    }

} // namespace node_osmium
