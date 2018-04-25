#include "precomp.h"
#include "Counter_wrap.h"
#include "icdcore/icd_item_Counter.h"

PROTOCORE_BEGIN

Napi::FunctionReference CounterWrap::ctor;

void CounterWrap::Initialize(Napi::Env env, Napi::Object exports)
{
    std::vector<PropertyDescriptor> properties = {
        ITEMWRAP_METHODS_PROPS(CounterWrap),
    };
    ctor = napi_init<CounterWrap, PROTOCORE_DOMAIN::ObjectWrap>(env, exports, "Counter", properties);
}

CounterWrap::CounterWrap(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<CounterWrap>(info)
    , d(napi_unwrap_data<Icd::CounterItem>(info, true))
{

}

ITEMWRAP_METHODS_IMPL(CounterWrap)

PROTOCORE_END
