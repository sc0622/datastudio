#pragma once

#include "node_global.h"
#include "item_wrap.h"

namespace Icd {
class CheckItem;
typedef std::shared_ptr<CheckItem> CheckItemPtr;
}

PROTOCORE_BEGIN

class CheckWrap : public Napi::ObjectWrap<CheckWrap>
{
public:
    static Napi::FunctionReference ctor;
    static void Initialize(Napi::Env env, Napi::Object exports);
    CheckWrap(const Napi::CallbackInfo &info);

    Icd::CheckItemPtr data() const { return d; }

    ITEMWRAP_METHODS_DECL();

    NAPI_GETTER_DECL(IsValid);
    NAPI_PROPERTY_DECL(CheckType);
    NAPI_GETTER_DECL(CheckTypeString);
    static NAPI_METHOD_DECL(CheckTypeString);
    static NAPI_METHOD_DECL(StringCheckType);
    NAPI_PROPERTY_DECL(StartPos);
    NAPI_PROPERTY_DECL(EndPos);
    NAPI_GETTER_DECL(CheckLength);

private:
    Icd::CheckItemPtr d;
};

PROTOCORE_END
