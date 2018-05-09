#include "precomp.h"
#include "limit_wrap.h"
#include "icdcore/icd_item_limit.h"

PROTOCORE_BEGIN

Napi::FunctionReference LimitWrap::ctor;

void LimitWrap::Initialize(Napi::Env env, Napi::Object exports)
{
    std::vector<PropertyDescriptor> properties = {
        InstanceAccessor("minimum", &LimitWrap::GetMinimum, &LimitWrap::SetMinimum, napi_enumerable),
        InstanceAccessor("maximum", &LimitWrap::GetMaximum, &LimitWrap::SetMaximum, napi_enumerable),
        InstanceAccessor("leftInf", &LimitWrap::GetLeftInf, &LimitWrap::SetLeftInf, napi_enumerable),
        InstanceAccessor("rightInf", &LimitWrap::GetRightInf, &LimitWrap::SetRightInf, napi_enumerable),
        InstanceMethod("toString", &LimitWrap::ToString, napi_enumerable),
    };
    ctor = napi_init<LimitWrap, PROTOCORE_DOMAIN::ObjectWrap>(env, exports, "Limit", properties);
}

LimitWrap::LimitWrap(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<LimitWrap>(info)
    , d(napi_unwrap_data<Icd::LimitItem>(info, true))
{

}

NAPI_GETTER(LimitWrap, Minimum) {
    return Napi::Number::New(info.Env(), d->minimum());
}

NAPI_SETTER(LimitWrap, Minimum) {
    d->setMinimum(info[0].As<Napi::Number>());
}

NAPI_GETTER(LimitWrap, Maximum) {
    return Napi::Number::New(info.Env(), d->maximum());
}

NAPI_SETTER(LimitWrap, Maximum) {
    d->setMaximum(info[0].As<Napi::Number>());
}

NAPI_GETTER(LimitWrap, LeftInf) {
    return Napi::Boolean::New(info.Env(), d->leftInf());
}

NAPI_SETTER(LimitWrap, LeftInf) {
    d->setLeftInf(info[0].As<Napi::Boolean>());
}

NAPI_GETTER(LimitWrap, RightInf) {
    return Napi::Boolean::New(info.Env(), d->rightInf());
}

NAPI_SETTER(LimitWrap, RightInf) {
    d->setRightInf(info[0].As<Napi::Boolean>());
}

NAPI_METHOD(LimitWrap, ToString) {
    return Napi::String::New(info.Env(), d->toString());
}

PROTOCORE_END
