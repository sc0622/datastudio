#pragma once

#include <node/napi.h>
#include <memory>

#ifndef PROTOCORE_BEGIN
#define PROTOCORE_BEGIN namespace protocore {
#endif

#ifndef PROTOCORE_END
#define PROTOCORE_END }
#endif

#ifndef PROTOCORE_PREFIX
#define PROTOCORE_PREFIX Icd
#endif

namespace Icd {
class Object;
typedef std::shared_ptr<Icd::Object> ObjectPtr;
}

//
typedef struct __SharedData { Icd::ObjectPtr ptr; } SharedData;
typedef SharedData *SharedDataPtr;

//
template<typename T>
inline std::shared_ptr<T> parse_object_data(void *data, bool create = false)
{
    if (!data) {
        return std::shared_ptr<T>();
    }

    auto d = reinterpret_cast<SharedDataPtr>(data);
    if (!d) {
        return std::shared_ptr<T>();
    }

    auto ptr = std::dynamic_pointer_cast<T>(d->ptr);
    if (!ptr && create) {
        ptr = std::shared_ptr<T>(new T());
    }

    return ptr;
}

/*
namespace node {

//
typedef v8::FunctionCallbackInfo<v8::Value> Args;
typedef const Args &CArgsRef;
typedef v8::Handle<v8::Object> Object;
typedef v8::Handle<v8::Value> Value;
typedef v8::Handle<v8::Array> Array;

inline std::string toStdString(const Value &value)
{
    return *v8::String::Utf8Value(value);
}

inline Value fromStdString(v8::Isolate* isolate, const std::string &str)
{
    return v8::String::NewFromUtf8(isolate, str.c_str());
}

}

#ifndef NODE_DECL
#define NODE_DECL(Class) \
    public: \
    static void Initialize(node::Object exports); \
    static v8::Local<v8::FunctionTemplate> tpl_;\
    private: \
    static v8::Persistent<v8::Function> constructor; \
    static void New(const node::Args &args);
#endif

#ifndef NODE_DECL2
#define NODE_DECL2(Class) \
    public: \
    static void Initialize(node::Object exports); \
    PROTOCORE_PREFIX::Class *wrapped() const { return d_; } \
    static v8::Local<v8::FunctionTemplate> tpl_;\
    private: \
    static v8::Persistent<v8::Function> constructor; \
    static void New(const node::Args &args); \
    PROTOCORE_PREFIX::Class *d_;
#endif

#ifndef NODE_IMPL
#define NODE_IMPL(Class) \
    v8::Persistent<v8::Function> Class::constructor
#endif

#ifndef NODE_IMPL2
#define NODE_IMPL2(Class) \
    v8::Persistent<v8::Function> Class##Wrap::constructor; \
    v8::Local<v8::FunctionTemplate> Class##Wrap::tpl_;
#endif

#ifndef NODE_NEW_INST
#define NODE_NEW_INST(argv) \
    do { \
    v8::Isolate* isolate = args.GetIsolate(); \
    v8::Local<v8::Context> context = isolate->GetCurrentContext(); \
    v8::Local<v8::Function> cons = v8::Local<v8::Function>::New(isolate, constructor); \
    v8::Local<v8::Object> result = cons->NewInstance(context, _countof(argv), argv).ToLocalChecked(); \
    args.GetReturnValue().Set(result); \
    } while(false)
#endif

#ifndef NODE_D
#define NODE_D(Class, args) \
    v8::Isolate *isolate = args.GetIsolate(); \
    Class##Wrap *dWrap = Unwrap<Class##Wrap>(args.Holder()); \
    PROTOCORE_PREFIX::Class *d = dWrap->wrapped()
#endif

#ifndef NODE_SET_RETURN
#define NODE_SET_RETURN(Type, value) \
    args.GetReturnValue().Set(v8::Type::New(isolate, (value)))
#endif

#ifndef NODE_SET_STR_RETURN
#define NODE_SET_STR_RETURN(value) \
    args.GetReturnValue().Set(node::toStdString(isolate, (value)))
#endif

//
#ifndef NODE_METHOD_DECL
#define NODE_METHOD_DECL(method) \
    static void method(node::CArgsRef args);
#endif

// [node-method-map] -->

#ifndef NODE_METHOD_MAP_BEGIN
#define NODE_METHOD_MAP_BEGIN(_class_, _module_) \
    void _class_::Initialize(node::Object exports) { \
    v8::Isolate* isolate = exports->GetIsolate(); \
    v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(isolate, New); \
    const v8::Local<v8::String> className = v8::String::NewFromUtf8(isolate, NODE_STRINGIFY(_module_)); \
    tpl->SetClassName(className); \
    tpl->InstanceTemplate()->SetInternalFieldCount(1); \
    tpl_ = tpl;
#endif

#ifndef NODE_METHOD_INHERIT
#define NODE_METHOD_INHERIT(_class_) \
    tpl->Inherit(_class_::tpl_);
#endif

#ifndef NODE_METHOD_MAP
#define NODE_METHOD_MAP(name, method) \
    NODE_SET_PROTOTYPE_METHOD(tpl, NODE_STRINGIFY(name), method);
#endif

#ifndef NODE_METHOD_MAP_END
#define NODE_METHOD_MAP_END() \
    constructor.Reset(isolate, tpl->GetFunction()); \
    exports->Set(className, tpl->GetFunction()); \
    }
#endif

// [node-method-map] <--
*/
// [napi-defines] -->

#ifndef NAPI_DECL_METHOD
#define NAPI_DECL_METHOD(method) \
    static napi_value method(napi_env env, napi_callback_info info)
#endif

#ifndef NAPI_DECL_OBJECT
#define NAPI_DECL_OBJECT(Class) \
    public: \
    static napi_value Init(napi_env env, napi_value exports); \
    static void Destructor(napi_env env, void* nativeObject, void* finalize_hint); \
    static napi_value NewInstance(napi_env env, const PROTOCORE_PREFIX::Class##Ptr data); \
    napi_env getEnv() { return env_; } \
    void setEnv(napi_env env) { env_ = env; }; \
    napi_ref &getWrapper() { return wrapper_; } \
    private: \
    ~Class##Wrap(); \
    static napi_value New(napi_env env, napi_callback_info info); \
    private: \
    static napi_ref ctor; \
    napi_env env_; \
    napi_ref wrapper_; \
    PROTOCORE_PREFIX::Class##Ptr data_;
#endif

#ifndef NAPI_IMPL_OBJECT
#define NAPI_IMPL_OBJECT(Class) \
    napi_ref Class##Wrap::ctor; \
    \
    Class##Wrap::~Class##Wrap() { napi_delete_reference(env_, wrapper_); } \
    \
    void Class##Wrap::Destructor(napi_env env, void* nativeObject, void*) { \
    reinterpret_cast<Class##Wrap*>(nativeObject)->~Class##Wrap(); } \
    \
    napi_value Class##Wrap::NewInstance(napi_env env, const PROTOCORE_PREFIX::Class##Ptr data) { \
    return napi_new_object<Class##Wrap, PROTOCORE_PREFIX::Class##Ptr>(env, ctor, data); \
    } \
    \
    napi_value Class##Wrap::New(napi_env env, napi_callback_info info) { \
    return napi_new_object<Class##Wrap, PROTOCORE_PREFIX::Class, PROTOCORE_PREFIX::Class##Ptr>(env, info, ctor); \
    }
#endif
namespace protocore {
template<typename T, typename HPTR>
inline napi_value napi_new_object(napi_env env, napi_ref ctor, const HPTR &data)
{
    napi_status status;
    struct Data { HPTR handle; } data_;
    data_.handle = data;
    const int argc = 1;
    napi_value argv[argc];
    napi_value cons;

    //FIXEDME [ finalize_cb ?= nullptr, to resolve ]
    if (napi_create_external(env, &data_, nullptr, nullptr, &argv[0]) != napi_ok) {
        return nullptr;
    }

    status = napi_get_reference_value(env, ctor, &cons);
    if (status != napi_ok)
        return nullptr;

    napi_value instance = nullptr;
    status = napi_new_instance(env, cons, argc, argv, &instance);
    if (status != napi_ok)
        return nullptr;

    return instance;
}

template<typename T, typename H, typename HPTR>
inline napi_value napi_new_object(napi_env env, napi_callback_info info, napi_ref ctor)
{
    napi_status status;
    napi_value target;
    status = napi_get_new_target(env, info, &target);
    assert(status == napi_ok);
    bool is_constructor = target != nullptr;

    if (is_constructor) {
        size_t argc = 1;
        napi_value args[1];
        napi_value jsthis;
        status = napi_get_cb_info(env, info, &argc, args, &jsthis, nullptr);
        assert(status == napi_ok);

        napi_valuetype valuetype;
        status = napi_typeof(env, args[0], &valuetype);
        assert(status == napi_ok);

        struct Data { HPTR handle; } *data_ = nullptr;
        if (valuetype != napi_undefined) {
            status = napi_get_value_external(env, args[0], reinterpret_cast<void**>(&data_));
            assert(status == napi_ok);
            if (!data_) {
                return nullptr;
            }
        }

        T *newObject = nullptr;
        if (!data_ || !data_->handle) {
            newObject = new T(HPTR(new H()));
        } else {
            newObject = new T(data_->handle);
        }

        newObject->setEnv(env);
        status = napi_wrap(env, jsthis, reinterpret_cast<void*>(newObject),
                           T::Destructor, nullptr, &newObject->getWrapper());
        assert(status == napi_ok);
        return jsthis;
    } else {
        size_t argc_ = 1;
        napi_value args[1];
        status = napi_get_cb_info(env, info, &argc_, args, nullptr, nullptr);
        assert(status == napi_ok);

        const size_t argc = 1;
        napi_value argv[argc] = { args[0] };

        napi_value cons;
        status = napi_get_reference_value(env, ctor, &cons);
        assert(status == napi_ok);

        napi_value instance;
        status = napi_new_instance(env, cons, argc, argv, &instance);
        assert(status == napi_ok);
        return instance;
    }
}

}

// [napi-method-map] -->

#ifndef NAPI_MAP_BEGIN
#define NAPI_MAP_BEGIN(Class) \
    napi_value Class##Wrap::Init(napi_env env, napi_value exports) { \
    const char *name = NODE_STRINGIFY(Class); \
    napi_status status; \
    napi_property_descriptor properties[] = {
#endif

#ifndef NAPI_MAP_END
#define NAPI_MAP_END() \
    }; \
    napi_value cons; \
    status = napi_define_class(env, name, NAPI_AUTO_LENGTH, New, nullptr, \
    _countof(properties), properties, &cons); \
    if (status != napi_ok) return exports; \
    \
    status = napi_create_reference(env, cons, 1, &ctor); \
    if (status != napi_ok) return exports; \
    \
    status = napi_set_named_property(env, exports, name, cons); \
    if (status != napi_ok) return exports; \
    return exports; \
    }
#endif

#ifndef NAPI_DESCR_PROPERTY
#define NAPI_DESCR_PROPERTY(name, get, set) \
{ NODE_STRINGIFY(name), nullptr, nullptr, get, set, nullptr, napi_default, nullptr },
#endif

#ifndef NAPI_DESCR_METHOD
#define NAPI_DESCR_METHOD(name, func) \
{ NODE_STRINGIFY(name), nullptr, func, nullptr, nullptr, nullptr, napi_default, nullptr }
#endif

#ifndef NAPI_METHO
#define NAPI_MEMBER_METHO(Class, Name) \
    napi_value Class::Name(napi_env env, napi_callback_info info)
#endif

#ifndef NAPI_D
#define NAPI_D(Class) \
    napi_value value = nullptr; \
    Class *wrap = nullptr; \
    do { \
    napi_status status; \
    size_t argc = 1; \
    napi_value jsthis; \
    status = napi_get_cb_info(env, info, &argc, &value, &jsthis, nullptr); \
    assert(status == napi_ok); \
    if (status != napi_ok) return nullptr; \
    \
    status = napi_unwrap(env, jsthis, reinterpret_cast<void**>(&wrap)); \
    assert(status == napi_ok); \
    if (status != napi_ok) return nullptr; \
    } while (false)
#endif

template <typename T>
inline T *napi_unwrap_object(napi_env env, napi_callback_info info)
{
    napi_status status;
    size_t argc = 1;
    napi_value value;
    napi_value jsthis;
    status = napi_get_cb_info(env, info, &argc, &value, &jsthis, nullptr);
    assert(status == napi_ok);
    if (status != napi_ok) return nullptr;

    T *object;
    status = napi_unwrap(env, jsthis, reinterpret_cast<void**>(&object));
    assert(status == napi_ok);
    if (status != napi_ok) return nullptr;

    return object;
}

// - get value -

inline napi_value napi_create_int32(napi_env env, int32_t value)
{
    napi_value _value;
    if (napi_create_int32(env, value, &_value) != napi_ok) {
        return nullptr;
    }

    return _value;
}

inline napi_value napi_create_string(napi_env env, const std::string &value)
{
    napi_value _value;
    if (napi_create_string_utf8(env, value.c_str(), value.size(), &_value) != napi_ok) {
        return nullptr;
    }

    return _value;
}

// - set value -

inline int32_t napi_get_int32(napi_env env, napi_value value)
{
    int32_t _value;
    if (napi_get_value_int32(env, value, &_value) != napi_ok) {
        return 0;
    }

    return _value;
}

inline std::string napi_get_string(napi_env env, napi_value value)
{
    size_t strlen = 0;
    if (napi_get_value_string_utf8(env, value, nullptr, 0, &strlen) != napi_ok
            || strlen == 0) {
        return std::string();
    }

    char *buffer = new char[strlen + 1];
    size_t res;
    napi_status status = napi_get_value_string_utf8(env, value, buffer, strlen + 1, &res);
    if (status != napi_ok) {
        delete[] buffer;
        return std::string();
    }

    const std::string _value(buffer, strlen);
    delete[] buffer;
    return _value;
}

// [napi-method-map] <--

// [napi-defines] <--
