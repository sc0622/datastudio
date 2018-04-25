#pragma once

#include "node_global.h"

namespace Icd {
class Object;
typedef std::shared_ptr<Object> ObjectPtr;
}

PROTOCORE_BEGIN

#ifndef OBJECTWRAP_METHODS_DECL
#define OBJECTWRAP_METHODS_DECL() \
    NAPI_VOID_METHOD_DECL(ResetData); \
    NAPI_VOID_METHOD_DECL(ClearData); \
    NAPI_METHOD_DECL(DomainOfType)
#endif

class ObjectWrap : public Napi::ObjectWrap<ObjectWrap>
{
public:
    static Napi::FunctionReference ctor;
    static void Initialize(Napi::Env env, Napi::Object exports);
    ObjectWrap(const Napi::CallbackInfo& info);
    Icd::ObjectPtr data() const { return d; }

    NAPI_GETTER_DECL(ObjectType);
    NAPI_PROPERTY_DECL(Id);
    NAPI_PROPERTY_DECL(Domain);
    NAPI_PROPERTY_DECL(Name);
    NAPI_PROPERTY_DECL(Mark);
    NAPI_PROPERTY_DECL(Desc);
    NAPI_GETTER_DECL(IsPrivateMark);
    NAPI_GETTER_DECL(ObjectTypeString);
    NAPI_GETTER_DECL(ChildCount);

    OBJECTWRAP_METHODS_DECL();

private:
    Icd::ObjectPtr d;
};

#ifndef OBJECTWRAP_METHODS_PROPS
#define OBJECTWRAP_METHODS_PROPS(Class) \
    InstanceMethod("resetData", &Class::ResetData, napi_enumerable), \
    InstanceMethod("clearData", &Class::ClearData, napi_enumerable), \
    InstanceMethod("domainOfType", &Class::DomainOfType, napi_enumerable)
#endif

#ifndef OBJECTWRAP_METHODS_IMPL
#define OBJECTWRAP_METHODS_IMPL(Class) \
    NAPI_VOID_METHOD(Class, ResetData) { \
    d->resetData(); } \
    NAPI_VOID_METHOD(Class, ClearData) { \
    d->clearData(); } \
    NAPI_METHOD(Class, DomainOfType) { \
    return Napi::String::New(info.Env(), d->domainOfType(info[0].As<Napi::Number>())); }
#endif

PROTOCORE_END
