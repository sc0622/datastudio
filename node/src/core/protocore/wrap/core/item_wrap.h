#pragma once

#include "node_global.h"
#include "object_wrap.h"

namespace Icd {
class Item;
typedef std::shared_ptr<Item> ItemPtr;
}

PROTOCORE_BEGIN

#ifndef ITEMWRAP_METHODS_DECL
#define ITEMWRAP_METHODS_DECL() \
    OBJECTWRAP_METHODS_DECL(); \
    NAPI_METHOD_DECL(StringType)
#endif

class ItemWrap : public Napi::ObjectWrap<ItemWrap>
{
public:
    static Napi::FunctionReference ctor;
    static void Initialize(Napi::Env env, Napi::Object exports);
    ItemWrap(const Napi::CallbackInfo &info);

    Icd::ItemPtr data() const { return d; }

    NAPI_GETTER_DECL(Type);
    //NAPI_GETTER_DECL(Buffer);
    NAPI_GETTER_DECL(BufferSize);
    NAPI_GETTER_DECL(BufferOffset);
    NAPI_PROPERTY_DECL(Data);
    NAPI_GETTER_DECL(DataString);
    NAPI_PROPERTY_DECL(DefaultValue);
    NAPI_GETTER_DECL(TypeName);
    NAPI_GETTER_DECL(CodeName);
    static NAPI_METHOD_DECL(Create);

    ITEMWRAP_METHODS_DECL();

    static Icd::ItemPtr fromObject(Napi::Env env, const Napi::Value &value);
    static Napi::Value toObject(Napi::Env env, const Icd::ItemPtr &data);

private:
    Icd::ItemPtr d;
};

#ifndef ITEMWRAP_METHODS_PROPS
#define ITEMWRAP_METHODS_PROPS(Class) \
    OBJECTWRAP_METHODS_PROPS(Class), \
    InstanceMethod("stringType", &Class::StringType, napi_enumerable)
#endif

#ifndef ITEMWRAP_METHODS_IMPL
#define ITEMWRAP_METHODS_IMPL(Class) \
    OBJECTWRAP_METHODS_IMPL(Class) \
    NAPI_METHOD(Class, StringType) { \
    if (info.Length() != 1) { \
        return napi_throwjs(Napi::Error::New(info.Env(), "Need one argument!")); \
        return Napi::Value(); \
    } \
    return Napi::Number::New(info.Env(), d->stringType(info[0].As<Napi::String>())); }
#endif

PROTOCORE_END
