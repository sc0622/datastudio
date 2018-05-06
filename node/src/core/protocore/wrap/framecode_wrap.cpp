#include "precomp.h"
#include "frameCode_wrap.h"
#include "icdcore/icd_item_framecode.h"
#include "icdcore/icd_item_frame.h"
#include "frame_wrap.h"

PROTOCORE_BEGIN

Napi::FunctionReference FrameCodeWrap::ctor;

void FrameCodeWrap::Initialize(Napi::Env env, Napi::Object exports)
{
    std::vector<PropertyDescriptor> properties = {
        ITEMWRAP_METHODS_PROPS(FrameCodeWrap),
        InstanceAccessor("frameCodeType", &FrameCodeWrap::GetFrameCodeType, &FrameCodeWrap::SetFrameCodeType, napi_enumerable),
        StaticMethod("frameCodeTypeString", &FrameCodeWrap::FrameCodeTypeString, napi_enumerable),
        StaticMethod("stringFrameCodeType", &FrameCodeWrap::StringFrameCodeType, napi_enumerable),
    };
    ctor = napi_init<FrameCodeWrap, PROTOCORE_DOMAIN::ObjectWrap>(env, exports, "FrameCode", properties);
}

FrameCodeWrap::FrameCodeWrap(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<FrameCodeWrap>(info)
    , d(napi_unwrap_data<Icd::FrameCodeItem>(info, true))
{

}

ITEMWRAP_METHODS_IMPL(FrameCodeWrap)

NAPI_GETTER(FrameCodeWrap, FrameCodeType) {
    return Napi::Number::New(info.Env(), d->frameCodeType());
}

NAPI_SETTER(FrameCodeWrap, FrameCodeType) {
    d->setFrameCodeType(Icd::FrameCodeType(info[0].As<Napi::Number>().Int32Value()));
}

NAPI_METHOD(FrameCodeWrap, FrameCodeTypeString) {
    if (info.Length() != 1) {
        NAPI_THROW(Napi::Error::New(info.Env(), "Need one argument!"));
        return Napi::Value();
    }
    return Napi::String::New(info.Env(), Icd::FrameCodeItem::frameCodeTypeString
                             (Icd::FrameCodeType(info[0].As<Napi::Number>().Int32Value())));
}

NAPI_METHOD(FrameCodeWrap, StringFrameCodeType) {
    if (info.Length() != 1) {
        NAPI_THROW(Napi::Error::New(info.Env(), "Need one argument!"));
        return Napi::Value();
    }
    return Napi::Number::New(info.Env(), Icd::FrameCodeItem::stringFrameCodeType(info[0].As<Napi::String>()));
}

NAPI_GETTER(FrameCodeWrap, FrameId) {
    return Napi::String::New(info.Env(), d->frameId());
}

NAPI_SETTER(FrameCodeWrap, FrameId) {
    d->setFrameId(info[0].As<Napi::String>());
}

NAPI_GETTER(FrameCodeWrap, Frame) {
    auto frame = d->frame();
    if (frame) {
        return napi_instance(info.Env(), FrameWrap::ctor, frame);
    } else {
        return Napi::Value();
    }
}

NAPI_SETTER(FrameCodeWrap, Frame) {
    FrameWrap *frameWrap = FrameWrap::Unwrap(info[0].As<Napi::Object>());
    if (frameWrap) {
        d->setFrame(frameWrap->data());
    }
}

NAPI_VOID_METHOD(FrameCodeWrap, UpdateSend) {
    if (info.Length() != 1) {
        NAPI_THROW(Napi::Error::New(info.Env(), "Need one argument!"));
        return;
    }
    d->updateSend(info[0].As<Napi::Boolean>());
}

NAPI_VOID_METHOD(FrameCodeWrap, UpdateRecv) {
    d->updateRecv();
}

NAPI_VOID_METHOD(FrameCodeWrap, ResetSend) {
    d->resetSend();
}

PROTOCORE_END
