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

    NAPI_PROPERTY_DECL(CounterType);
    NAPI_GETTER_DECL(CounterTypeString);
    static NAPI_METHOD_DECL(CounterTypeString);
    static NAPI_METHOD_DECL(StringCounterType);
    NAPI_PROPERTY_DECL(Value);

private:
    Icd::CounterItemPtr d;
};

PROTOCORE_END
