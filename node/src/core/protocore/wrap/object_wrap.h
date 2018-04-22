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
    BaseObject(const Napi::CallbackInfo& info);

    Napi::Value objectType(const Napi::CallbackInfo &info);

    //NAPI_DECL_METHOD(ObjectType);
    //NAPI_DECL_METHOD(GetId);
    //NAPI_DECL_METHOD(SetId);

    /*NAPI_METHOD_DECL(Parent);
    NAPI_METHOD_DECL(SetParent);

    NAPI_METHOD_DECL(Domain);
    NAPI_METHOD_DECL(Name);
    NAPI_METHOD_DECL(Mark);
    NAPI_METHOD_DECL(Desc);

    NAPI_METHOD_DECL(SetId);
    NAPI_METHOD_DECL(SetDomain);
    NAPI_METHOD_DECL(SetName);
    NAPI_METHOD_DECL(SetMark);
    NAPI_METHOD_DECL(SetDesc);

    NAPI_METHOD_DECL(IsPrivateMark);
    NAPI_METHOD_DECL(ChildCount);
    NAPI_METHOD_DECL(ObjectTypeString);
    NAPI_METHOD_DECL(ResetData);
    NAPI_METHOD_DECL(ClearData);
    NAPI_METHOD_DECL(Clone);
    NAPI_METHOD_DECL(DomainOfType);*/

    static void Initialize(Napi::Env env, Napi::Object exports);
    static Napi::Object New(Napi::Env env, const Icd::ObjectPtr &data);

private:
    Icd::ObjectPtr data;
    Napi::FunctionReference Constructor;
};

PROTOCORE_END
