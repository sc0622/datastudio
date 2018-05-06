#pragma once

#include "node_global.h"
#include "item_wrap.h"

namespace Icd {
class NumericItem;
typedef std::shared_ptr<NumericItem> NumericItemPtr;
}

PROTOCORE_BEGIN

class NumericWrap : public Napi::ObjectWrap<NumericWrap>
{
public:
    static Napi::FunctionReference ctor;
    static void Initialize(Napi::Env env, Napi::Object exports);
    NumericWrap(const Napi::CallbackInfo &info);

    Icd::NumericItemPtr data() const { return d; }

    ITEMWRAP_METHODS_DECL();

    NAPI_PROPERTY_DECL(NumericType);
    NAPI_GETTER_DECL(NumericTypeString);
    static NAPI_METHOD_DECL(NumericTypeString);
    static NAPI_METHOD_DECL(StringNumericType);
    NAPI_GETTER_DECL(OriginalData);
    NAPI_PROPERTY_DECL(Scale);
    NAPI_PROPERTY_DECL(Decimals);
    NAPI_PROPERTY_DECL(Offset);
    NAPI_PROPERTY_DECL(Limit);
    NAPI_PROPERTY_DECL(Unit);
    NAPI_GETTER_DECL(Specs);
    NAPI_VOID_METHOD_DECL(AddSpec);
    NAPI_METHOD_DECL(SpecAt);
    NAPI_VOID_METHOD_DECL(RemoveSpec);
    NAPI_VOID_METHOD_DECL(ClearSpec);
    NAPI_GETTER_DECL(DataRange);
    NAPI_GETTER_DECL(ValueRange);
    NAPI_GETTER_DECL(OutOfLimit);

private:
    Icd::NumericItemPtr d;
};

PROTOCORE_END
