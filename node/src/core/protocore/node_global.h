#pragma once

#include <node/node.h>
#include <node/node_object_wrap.h>
#include <node/node_api.h>

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

#ifndef PROTOCORE_BEGIN
#define PROTOCORE_BEGIN namespace protocore {
#endif

#ifndef PROTOCORE_END
#define PROTOCORE_END }
#endif

#ifndef PROTOCORE_PREFIX
#define PROTOCORE_PREFIX Icd
#endif

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
