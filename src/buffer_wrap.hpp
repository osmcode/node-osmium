#ifndef BUFFER_WRAP_HPP
#define BUFFER_WRAP_HPP

// c++
#include <memory>

#include "include_nan.hpp"

// osmium
#include <osmium/memory/buffer.hpp>

namespace node_osmium {

    class BufferWrap : public Nan::ObjectWrap {

        osmium::memory::Buffer m_this;
        osmium::memory::Buffer::iterator m_iterator;

        static NAN_METHOD(clear);
        static NAN_METHOD(next);
        static NAN_METHOD(filter_point_in_time);
        static NAN_METHOD(create_node_buffer);

    public:

        static Nan::Persistent<v8::FunctionTemplate> constructor;
        static void Initialize(v8::Handle<v8::Object> target);
        static NAN_METHOD(New);

        BufferWrap(osmium::memory::Buffer&& buffer) :
            Nan::ObjectWrap(),
            m_this(std::move(buffer)),
            m_iterator() {
            if (m_this) {
                m_iterator = m_this.begin();
            }
        }

        osmium::memory::Buffer& get() {
            return m_this;
        }

    private:

        ~BufferWrap() = default;

    }; // class BufferWrap

} // namespace node_osmium

#endif //  BUFFER_WRAP_HPP
