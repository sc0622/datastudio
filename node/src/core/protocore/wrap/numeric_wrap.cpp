#include "precomp.h"
#include "Numeric_wrap.h"
#include "icdcore/icd_item_Numeric.h"

PROTOCORE_BEGIN

Napi::FunctionReference NumericWrap::ctor;

void NumericWrap::Initialize(Napi::Env env, Napi::Object exports)
{
    std::vector<PropertyDescriptor> properties = {
        ITEMWRAP_METHODS_PROPS(NumericWrap),
    };
    ctor = napi_init<NumericWrap, PROTOCORE_DOMAIN::ObjectWrap>(env, exports, "Numeric", properties);
}

NumericWrap::NumericWrap(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<NumericWrap>(info)
    , d(napi_unwrap_data<Icd::NumericItem>(info, true))
{

}

ITEMWRAP_METHODS_IMPL(NumericWrap)

PROTOCORE_END
