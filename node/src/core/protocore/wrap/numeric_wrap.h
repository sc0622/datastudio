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

private:
    Icd::NumericItemPtr d;
};

PROTOCORE_END
