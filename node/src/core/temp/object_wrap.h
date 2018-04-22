#pragma once

#include "node_global.h"

namespace Icd { class Object; }

PROTOCORE_BEGIN

class ObjectWrap : public node::ObjectWrap
{
public:
    ObjectWrap(Icd::Object *data);

private:
    NODE_METHOD_DECL(Parent)
    NODE_METHOD_DECL(SetParent)

    NODE_METHOD_DECL(ObjectType)
    NODE_METHOD_DECL(Id)
    NODE_METHOD_DECL(Domain)
    NODE_METHOD_DECL(Name)
    NODE_METHOD_DECL(Mark)
    NODE_METHOD_DECL(Desc)

    NODE_METHOD_DECL(SetId)
    NODE_METHOD_DECL(SetDomain)
    NODE_METHOD_DECL(SetName)
    NODE_METHOD_DECL(SetMark)
    NODE_METHOD_DECL(SetDesc)

    NODE_METHOD_DECL(IsPrivateMark)
    NODE_METHOD_DECL(ChildCount)
    NODE_METHOD_DECL(ObjectTypeString)
    NODE_METHOD_DECL(ResetData)
    NODE_METHOD_DECL(ClearData)
    NODE_METHOD_DECL(Clone)
    NODE_METHOD_DECL(DomainOfType)

    private:
        NODE_DECL2(Object)
};
    //

    class MyObject
    {
    public:
        static napi_value Init(napi_env env, napi_value exports);
        static void Destructor(napi_env env, void* nativeObject, void* finalize_hint);
        static napi_status NewInstance(napi_env env,
                                       napi_value arg,
                                       napi_value* instance);

    private:
        explicit MyObject(double value_ = 0);
        ~MyObject();

        static napi_value New(napi_env env, napi_callback_info info);
        static napi_value GetValue(napi_env env, napi_callback_info info);
        static napi_value SetValue(napi_env env, napi_callback_info info);
        static napi_value PlusOne(napi_env env, napi_callback_info info);
        static napi_value Multiply(napi_env env, napi_callback_info info);
        static napi_ref constructor;
        double value_;
        napi_env env_;
        napi_ref wrapper_;
    };


    PROTOCORE_END
