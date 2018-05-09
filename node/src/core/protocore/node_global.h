#pragma once

#include <napi.h>
#include <memory>
#include <iostream>

#ifndef PROTOCORE_DOMAIN
#define PROTOCORE_DOMAIN protocore
#endif

#ifndef PROTOCORE_BEGIN
#define PROTOCORE_BEGIN namespace PROTOCORE_DOMAIN {
#endif

#ifndef PROTOCORE_END
#define PROTOCORE_END }
#endif

#ifndef PROTOCORE_PREFIX
#define PROTOCORE_PREFIX Icd
#endif

#ifndef NAPI_GETTER_DECL
#define NAPI_GETTER_DECL(_name_) Napi::Value Get##_name_(const Napi::CallbackInfo &info)
#endif

#ifndef NAPI_SETTER_DECL
#define NAPI_SETTER_DECL(_name_) void Set##_name_(const Napi::CallbackInfo &info, const Napi::Value &value)
#endif

#ifndef NAPI_GETTER
#define NAPI_GETTER(_class_, _name_) Napi::Value _class_::Get##_name_(const Napi::CallbackInfo &info)
#endif

#ifndef NAPI_SETTER
#define NAPI_SETTER(_class_, _name_) void _class_::Set##_name_(const Napi::CallbackInfo &info, const Napi::Value &value)
#endif

#ifndef NAPI_METHOD_DECL
#define NAPI_METHOD_DECL(_name_) Napi::Value _name_(const Napi::CallbackInfo &info)
#endif

#ifndef NAPI_VOID_METHOD_DECL
#define NAPI_VOID_METHOD_DECL(_name_) void _name_(const Napi::CallbackInfo &info)
#endif

#ifndef NAPI_METHOD
#define NAPI_METHOD(_class_, _name_) Napi::Value _class_::_name_(const Napi::CallbackInfo &info)
#endif

#ifndef NAPI_VOID_METHOD
#define NAPI_VOID_METHOD(_class_, _name_) void _class_::_name_(const Napi::CallbackInfo &info)
#endif

#ifndef NAPI_PROPERTY_DECL
#define NAPI_PROPERTY_DECL(_name_) \
    NAPI_GETTER_DECL(_name_); \
    NAPI_SETTER_DECL(_name_)
#endif
// Throw an error with a specified message if the N-API call fails.
#define NAPI_CALL_BASE(env, call, message, return_value)                       \
{                                                                            \
    napi_status status = call;                                                 \
    if (status != napi_ok) {                                                   \
    napi_throw_error((env), nullptr, (message));                                \
    return return_value;                                                     \
    }                                                                          \
    }

inline Napi::Value napi_throwjs(Napi::Error &error) throw (Napi::Error) {
    error.ThrowAsJavaScriptException();
    return Napi::Boolean::New(error.Env(), false);
}

#define NAPI_CALL(env, call, message) \
    NAPI_CALL_BASE((env), (call), (message), nullptr)

// The last parameter is intentionally empty.
#define NAPI_CALL_VOID(env, call, message) \
    NAPI_CALL_BASE((env), (call), (message),)

// Add a property whose name is stored in prefix to the value stored either in
// this, or in the first argument. The value of the property will be `true`.
#define SET_TARGET_PROPERTY(env, use_this, info, prefix)                       \
{                                                                            \
    napi_value target, js_true;                                                \
    if (use_this) {                                                            \
    NAPI_CALL((env),                                                         \
    napi_get_cb_info((env), (info), nullptr, nullptr, &target, nullptr),    \
    prefix ": Failed to retrieve target via callback info");       \
    } else {                                                                   \
    size_t argc = 1;                                                         \
    NAPI_CALL((env),                                                         \
    napi_get_cb_info((env), (info), &argc, &target, nullptr, nullptr),   \
    prefix ": Failed to retrieve target via callback info");       \
    }                                                                          \
    NAPI_CALL(env, napi_get_boolean((env), true, &js_true),                    \
    prefix ": Failed to create boolean value");                      \
    NAPI_CALL(env, napi_set_named_property((env), target, (prefix), js_true),  \
    prefix ": Failed to set property");                              \
    }

namespace Icd {
class Object;
typedef std::shared_ptr<Icd::Object> ObjectPtr;
}

//
template<typename T>
inline std::shared_ptr<T> napi_unwrap_data(const Napi::CallbackInfo &info)
{
    std::shared_ptr<T> ptr;
    if (info.Length() > 0) {
        struct _Shared { std::shared_ptr<T> ptr; };
        auto d = info[0].As<Napi::External<struct _Shared> >().Data();
        if (d) {
            ptr = std::dynamic_pointer_cast<T>(d->ptr);
        }
    }

    return ptr;
}

//
template<typename T>
inline std::shared_ptr<T> napi_unwrap_data(const Napi::CallbackInfo &info, bool create)
{
    std::shared_ptr<T> ptr;
    if (info.Length() > 0) {
        struct _Shared { std::shared_ptr<T> ptr; };
        auto d = info[0].As<Napi::External<struct _Shared> >().Data();
        if (d) {
            ptr = std::dynamic_pointer_cast<T>(d->ptr);
        }
    }

    if (!ptr && create) {
        ptr = std::shared_ptr<T>(new T());
    }

    return ptr;
}

inline void napi_inherits(napi_env env, napi_value ctor, napi_value super_ctor)
{
    napi_value global, global_object, set_proto, ctor_proto_prop,
            super_ctor_proto_prop;
    napi_value args[2];

    NAPI_CALL_VOID(env, napi_get_global(env, &global),
                   "napi_inherits: Failed to retrieve global object");
    NAPI_CALL_VOID(env, napi_get_named_property(env, global, "Object",
                                                &global_object),
                   "napi_inherits: Failed to retrieve 'Object' from global");
    NAPI_CALL_VOID(env, napi_get_named_property(env, global_object,
                                                "setPrototypeOf", &set_proto),
                   "napi_inherits: Failed to retrieve 'setPrototypeOf' from 'Object'");
    NAPI_CALL_VOID(env,
                   napi_get_named_property(env, ctor, "prototype",
                                           &ctor_proto_prop),
                   "napi_inherits: Failed to retrieve 'prototype' from subclass");
    NAPI_CALL_VOID(env, napi_get_named_property(env, super_ctor, "prototype",
                                                &super_ctor_proto_prop),
                   "napi_inherits: Failed to retrieve 'prototype' from superclass");

    args[0] = ctor_proto_prop;
    args[1] = super_ctor_proto_prop;
    NAPI_CALL_VOID(env, napi_call_function(env, global, set_proto, 2, args, nullptr),
                   "napi_inherits: Failed to call 'setPrototypeOf' for prototypes");

    args[0] = ctor;
    args[1] = super_ctor;
    NAPI_CALL_VOID(env, napi_call_function(env, global, set_proto, 2, args, nullptr),
                   "napi_inherits: Failed to call 'setPrototypeOf' for constructors");
}

inline void napi_inherits(Napi::Env env, Napi::Function subCtor, const Napi::FunctionReference &superCtor)
{
    napi_inherits(napi_env(env), napi_value(subCtor), napi_value(superCtor.Value()));
}

inline Napi::Object napi_instance(Napi::Env env, const Napi::FunctionReference &ctor,
                                  const Icd::ObjectPtr &data)
{
    struct _Shared { std::shared_ptr<Icd::Object> ptr; } shared { data };
    auto inst = ctor.New({ Napi::External<struct _Shared>::New(env, &shared) });
    return inst;
}

template<typename T>
inline Napi::Object napi_instance(Napi::Env env, const Napi::FunctionReference &ctor,
                                  const std::shared_ptr<T> &data)
{
    struct _Shared { std::shared_ptr<T> ptr; } shared { data };
    auto inst = ctor.New({ Napi::External<struct _Shared>::New(env, &shared) });
    return inst;
}

template<typename Sub>
inline Napi::FunctionReference napi_init(Napi::Env env, Napi::Object exports, const char *name,
                                         const std::vector<Napi::ClassPropertyDescriptor<Sub> > &properties)
{
    Napi::HandleScope scope(env);
    Napi::Function object = Sub::DefineClass(env, name, properties);
    exports.Set(name, object);
    Napi::FunctionReference ctor = Napi::Persistent(object);
    ctor.SuppressDestruct();
    return ctor;
}

template<typename Sub, typename Super>
inline Napi::FunctionReference napi_init(Napi::Env env, Napi::Object exports, const char *name,
                                         const std::vector<Napi::ClassPropertyDescriptor<Sub> > &properties)
{
    Napi::HandleScope scope(env);
    Napi::Function object = Sub::DefineClass(env, name, properties);
    napi_inherits(env, object, Super::ctor);
    exports.Set(name, object);
    Napi::FunctionReference ctor = Napi::Persistent(object);
    ctor.SuppressDestruct();
    return ctor;
}

//

namespace Napi {

// argument error exception
class ArgumentError : public Error
{
public:
    static ArgumentError New(napi_env env, const char* message) {
        return Error::New<ArgumentError>(env, message, std::strlen(message), napi_create_error);
    }
    static ArgumentError New(napi_env env, const std::string& message) {
        return Error::New<ArgumentError>(env, message.c_str(), message.size(), napi_create_error);
    }

    ArgumentError() : Error() {}
    ArgumentError(napi_env env, napi_value value) : Error(env, value) {}
};

}
