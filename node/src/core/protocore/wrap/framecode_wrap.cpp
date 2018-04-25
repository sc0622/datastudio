#include "precomp.h"
#include "FrameCode_wrap.h"
#include "icdcore/icd_item_FrameCode.h"

PROTOCORE_BEGIN

Napi::FunctionReference FrameCodeWrap::ctor;

void FrameCodeWrap::Initialize(Napi::Env env, Napi::Object exports)
{
    std::vector<PropertyDescriptor> properties = {
        ITEMWRAP_METHODS_PROPS(FrameCodeWrap),
    };
    ctor = napi_init<FrameCodeWrap, PROTOCORE_DOMAIN::ObjectWrap>(env, exports, "FrameCode", properties);
}

FrameCodeWrap::FrameCodeWrap(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<FrameCodeWrap>(info)
    , d(napi_unwrap_data<Icd::FrameCodeItem>(info, true))
{

}

ITEMWRAP_METHODS_IMPL(FrameCodeWrap)

PROTOCORE_END
