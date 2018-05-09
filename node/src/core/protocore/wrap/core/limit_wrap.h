#pragma once

#include "node_global.h"
#include "item_wrap.h"

namespace Icd {
class LimitItem;
typedef std::shared_ptr<LimitItem> LimitItemPtr;
}

PROTOCORE_BEGIN

class LimitWrap : public Napi::ObjectWrap<LimitWrap>
{
public:
    static Napi::FunctionReference ctor;
    static void Initialize(Napi::Env env, Napi::Object exports);
    LimitWrap(const Napi::CallbackInfo &info);

    Icd::LimitItemPtr data() const { return d; }

    NAPI_PROPERTY_DECL(Minimum);
    NAPI_PROPERTY_DECL(Maximum);
    NAPI_PROPERTY_DECL(LeftInf);
    NAPI_PROPERTY_DECL(RightInf);
    NAPI_METHOD_DECL(ToString);

private:
    Icd::LimitItemPtr d;
};

PROTOCORE_END
