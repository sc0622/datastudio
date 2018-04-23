#pragma once

#include <node/napi.h>
#include <memory>
#include <iostream>

#ifndef PROTOCORE_BEGIN
#define PROTOCORE_BEGIN namespace protocore {
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

namespace Icd {
class Object;
typedef std::shared_ptr<Icd::Object> ObjectPtr;
}

//
typedef struct __napi_shared_data { Icd::ObjectPtr ptr; } napi_shared_data;
typedef napi_shared_data *napi_shared_data_ptr;

//
template<typename T>
inline std::shared_ptr<T> napi_unwrap_data(const Napi::CallbackInfo &info, bool create = false)
{
    std::shared_ptr<T> ptr;
    if (info.Length() > 0) {
        auto d = info[0].As<Napi::External<napi_shared_data> >().Data();
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
    napi_value global, global_object, set_proto, ctor_proto_prop, super_ctor_proto_prop;
    napi_value argv[2];

    napi_get_global(env, &global);
    napi_get_named_property(env, global, "Object", &global_object);
    napi_get_named_property(env, global_object, "setPrototypeOf", &set_proto);
    napi_get_named_property(env, ctor, "prototype", &ctor_proto_prop);
    napi_get_named_property(env, super_ctor, "prototype", &super_ctor_proto_prop);

    argv[0] = ctor_proto_prop;
    argv[1] = super_ctor_proto_prop;
    napi_call_function(env, global, set_proto, 2, argv, nullptr);

    argv[0] = ctor;
    argv[1] = super_ctor;
    napi_call_function(env, global, set_proto, 2, argv, nullptr);
}

inline void napi_inherits(Napi::Env env, Napi::Function subCtor, const Napi::FunctionReference &superCtor)
{
    napi_inherits(napi_env(env), napi_value(subCtor), napi_value(superCtor.Value()));
}

inline Napi::Object napi_instance(Napi::Env env, const Napi::FunctionReference &ctor, const Icd::ObjectPtr &data)
{
    napi_shared_data shared{ data };
    auto inst = ctor.New({ Napi::External<napi_shared_data>::New(env, &shared) });
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
    napi_inherits(env, object, Super::ctor.Value());
    exports.Set(name, object);
    Napi::FunctionReference ctor = Napi::Persistent(object);
    ctor.SuppressDestruct();
    return ctor;
}

