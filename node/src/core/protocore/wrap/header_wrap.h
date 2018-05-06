#pragma once

#include "node_global.h"
#include "item_wrap.h"

namespace Icd {
class HeaderItem;
typedef std::shared_ptr<HeaderItem> HeaderItemPtr;
}

PROTOCORE_BEGIN

class HeaderWrap : public Napi::ObjectWrap<HeaderWrap>
{
public:
    static Napi::FunctionReference ctor;
    static void Initialize(Napi::Env env, Napi::Object exports);
    HeaderWrap(const Napi::CallbackInfo &info);

    Icd::HeaderItemPtr data() const { return d; }

    ITEMWRAP_METHODS_DECL();

    NAPI_PROPERTY_DECL(Value);

private:
    Icd::HeaderItemPtr d;
};

PROTOCORE_END
