#include "precomp.h"
#include "Header_wrap.h"
#include "icdcore/icd_item_Header.h"

PROTOCORE_BEGIN

Napi::FunctionReference HeaderWrap::ctor;

void HeaderWrap::Initialize(Napi::Env env, Napi::Object exports)
{
    std::vector<PropertyDescriptor> properties = {
        ITEMWRAP_METHODS_PROPS(HeaderWrap),
    };
    ctor = napi_init<HeaderWrap, PROTOCORE_DOMAIN::ObjectWrap>(env, exports, "Header", properties);
}

HeaderWrap::HeaderWrap(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<HeaderWrap>(info)
    , d(napi_unwrap_data<Icd::HeaderItem>(info, true))
{

}

ITEMWRAP_METHODS_IMPL(HeaderWrap)

PROTOCORE_END
