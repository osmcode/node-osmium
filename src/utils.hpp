#ifndef UTILS_HPP
#define UTILS_HPP

// c++
#include <utility>

#include "include_nan.hpp"

// osmium
#include <osmium/osm/entity_bits.hpp>

namespace osmium {
    class Box;
}

// This is a check for the extra paranoid: In Javascript it is possible to call
// any method of any object with "this" of a totally different type of object
// using the apply() function:
//   some_object.some_method.apply(different_object);
// This will crash any C++ function that expects a certain type of object. With
// this macro we can check whether we have the right kind of object.
#define INSTANCE_CHECK(cpp_class, js_class, method) \
if (!Nan::New(cpp_class::constructor)->HasInstance(info.This())) { \
    Nan::ThrowTypeError(Nan::New("You can only call " method "() on an osmium." js_class ).ToLocalChecked()); \
    return ; \
}

namespace node_osmium {

    template<class T>
    auto unwrap(const v8::Local<v8::Object>& object) -> decltype(Nan::ObjectWrap::Unwrap<T>(object)->get()) {
        return Nan::ObjectWrap::Unwrap<T>(object)->get();
    }

    template<class T, class... Args>
    v8::Local<v8::Object> new_external(Args&&... args) {
        Nan::EscapableHandleScope scope;
        v8::Local<v8::Value> ext = Nan::New<v8::External>(new T(std::forward<Args>(args)...));
        v8::Local<v8::Function> f = Nan::GetFunction(Nan::New(T::constructor)).ToLocalChecked();
        Nan::MaybeLocal<v8::Object> maybe_local = Nan::NewInstance(f, 1, &ext);
        if (maybe_local.IsEmpty()) Nan::ThrowError("Could not create new Buffer instance");
        return scope.Escape(maybe_local.ToLocalChecked());

    }

    v8::Local<v8::Value> create_js_box(const osmium::Box& box);

    osmium::osm_entity_bits::type object_to_entity_bits(v8::Local<v8::Object> options);

} // namespace node_osmium

#endif // UTILS_HPP
