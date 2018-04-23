#pragma once

#include "node_global.h"

namespace Icd {
class Object;
typedef std::shared_ptr<Icd::Object> ObjectPtr;
}

PROTOCORE_BEGIN

class BaseObject : public Napi::ObjectWrap<BaseObject>
{
public:
    static Napi::FunctionReference ctor;
    static void Initialize(Napi::Env env, Napi::Object exports);
    BaseObject(const Napi::CallbackInfo& info);

    NAPI_GETTER_DECL(ObjectType);
    NAPI_PROPERTY_DECL(Id);
    NAPI_PROPERTY_DECL(Domain);
    NAPI_PROPERTY_DECL(Name);
    NAPI_PROPERTY_DECL(Mark);
    NAPI_PROPERTY_DECL(Desc);
    NAPI_METHOD_DECL(IsPrivateMark);
    NAPI_METHOD_DECL(ChildCount);
    NAPI_METHOD_DECL(ObjectTypeString);
    NAPI_VOID_METHOD_DECL(ResetData);
    NAPI_VOID_METHOD_DECL(ClearData);
    NAPI_METHOD_DECL(DomainOfType);

private:
    Icd::ObjectPtr data;
};

PROTOCORE_END
