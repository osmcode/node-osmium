#ifndef UTILS_HPP
#define UTILS_HPP

// c++
#include <utility>

// v8/node
#include "include_v8.hpp"
#include <node_object_wrap.h>

// osmium
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
if (!cpp_class::constructor->HasInstance(args.This())) { \
    return ThrowException(v8::Exception::TypeError(v8::String::New("You can only call " method "() on an osmium." js_class ))); \
}

namespace node_osmium {

    template<class T>
    auto unwrap(const v8::Local<v8::Object>& object) -> decltype(node::ObjectWrap::Unwrap<T>(object)->get()) {
        return node::ObjectWrap::Unwrap<T>(object)->get();
    }

    template<class T, class... Args>
    v8::Local<v8::Object> new_external(Args&&... args) {
        v8::HandleScope scope;
        v8::Handle<v8::Value> ext = v8::External::New(new T(std::forward<Args>(args)...));
        return scope.Close(T::constructor->GetFunction()->NewInstance(1, &ext));
    }

    v8::Handle<v8::Value> create_js_box(const osmium::Box& box);

} // namespace node_osmium

#endif // UTILS_HPP
