#pragma once

#include "node_global.h"
#include "item_wrap.h"

namespace Icd {
class CounterItem;
typedef std::shared_ptr<CounterItem> CounterItemPtr;
}

PROTOCORE_BEGIN

class CounterWrap : public Napi::ObjectWrap<CounterWrap>
{
public:
    static Napi::FunctionReference ctor;
    static void Initialize(Napi::Env env, Napi::Object exports);
    CounterWrap(const Napi::CallbackInfo &info);

    Icd::CounterItemPtr data() const { return d; }

    ITEMWRAP_METHODS_DECL();

private:
    Icd::CounterItemPtr d;
};

PROTOCORE_END
