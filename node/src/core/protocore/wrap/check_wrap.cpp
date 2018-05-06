#include "precomp.h"
#include "Check_wrap.h"
#include "icdcore/icd_item_check.h"

PROTOCORE_BEGIN

Napi::FunctionReference CheckWrap::ctor;

void CheckWrap::Initialize(Napi::Env env, Napi::Object exports)
{
    std::vector<PropertyDescriptor> properties = {
        ITEMWRAP_METHODS_PROPS(CheckWrap),
        InstanceAccessor("isValid", &CheckWrap::GetIsValid, nullptr, napi_enumerable),
        InstanceAccessor("checkType", &CheckWrap::GetCheckType, &CheckWrap::SetCheckType, napi_enumerable),
        StaticMethod("checkTypeString", &CheckWrap::CheckTypeString, napi_enumerable),
        StaticMethod("stringCheckType", &CheckWrap::StringCheckType, napi_enumerable),
        InstanceAccessor("startPos", &CheckWrap::GetStartPos, &CheckWrap::SetStartPos, napi_enumerable),
        InstanceAccessor("endPos", &CheckWrap::GetEndPos, &CheckWrap::SetEndPos, napi_enumerable),
        InstanceAccessor("checkLength", &CheckWrap::GetCheckLength, nullptr, napi_enumerable)
    };
    ctor = napi_init<CheckWrap, PROTOCORE_DOMAIN::ObjectWrap>(env, exports, "Check", properties);
}

CheckWrap::CheckWrap(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<CheckWrap>(info)
    , d(napi_unwrap_data<Icd::CheckItem>(info, true))
{

}

ITEMWRAP_METHODS_IMPL(CheckWrap)

NAPI_GETTER(CheckWrap, IsValid) {
    return Napi::Boolean::New(info.Env(), d->isValid());
}

NAPI_GETTER(CheckWrap, CheckType) {
    return Napi::Number::New(info.Env(), d->checkType());
}

NAPI_SETTER(CheckWrap, CheckType) {
    d->setCheckType(Icd::CheckType(info[0].As<Napi::Number>().Int32Value()));
}

NAPI_METHOD(CheckWrap, CheckTypeString) {
    if (info.Length() != 1) {
        NAPI_THROW(Napi::Error::New(info.Env(), "Need one argument!"));
        return Napi::Value();
    }
    return Napi::String::New(info.Env(), Icd::CheckItem::checkTypeString
                             (Icd::CheckType(info[0].As<Napi::Number>().Int32Value())));
}

NAPI_METHOD(CheckWrap, StringCheckType) {
    if (info.Length() != 1) {
        NAPI_THROW(Napi::Error::New(info.Env(), "Need one argument!"));
        return Napi::Value();
    }
    return Napi::Number::New(info.Env(), Icd::CheckItem::stringCheckType(info[0].As<Napi::String>()));
}

NAPI_GETTER(CheckWrap, StartPos) {
    return Napi::Number::New(info.Env(), d->startPos());
}

NAPI_SETTER(CheckWrap, StartPos) {
    d->setStartPos(info[0].As<Napi::Number>());
}

NAPI_GETTER(CheckWrap, EndPos) {
    return Napi::Number::New(info.Env(), d->endPos());
}

NAPI_SETTER(CheckWrap, EndPos) {
    d->setEndPos(info[0].As<Napi::Number>());
}

NAPI_GETTER(CheckWrap, CheckLength) {
    return Napi::Number::New(info.Env(), d->checkLength());
}

PROTOCORE_END
