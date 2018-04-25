#include "precomp.h"
#include "Frame_wrap.h"
#include "icdcore/icd_item_Frame.h"

PROTOCORE_BEGIN

Napi::FunctionReference FrameWrap::ctor;

void FrameWrap::Initialize(Napi::Env env, Napi::Object exports)
{
    std::vector<PropertyDescriptor> properties = {
        ITEMWRAP_METHODS_PROPS(FrameWrap),
    };
    ctor = napi_init<FrameWrap, PROTOCORE_DOMAIN::ObjectWrap>(env, exports, "Frame", properties);
}

FrameWrap::FrameWrap(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<FrameWrap>(info)
    , d(napi_unwrap_data<Icd::FrameItem>(info, true))
{

}

ITEMWRAP_METHODS_IMPL(FrameWrap)

PROTOCORE_END
