#include "precomp.h"
#include "Counter_wrap.h"
#include "icdcore/icd_item_counter.h"

PROTOCORE_BEGIN

Napi::FunctionReference CounterWrap::ctor;

void CounterWrap::Initialize(Napi::Env env, Napi::Object exports)
{
    std::vector<PropertyDescriptor> properties = {
        ITEMWRAP_METHODS_PROPS(CounterWrap),
        InstanceAccessor("counterType", &CounterWrap::GetCounterType, &CounterWrap::SetCounterType, napi_enumerable),
        StaticMethod("counterTypeString", &CounterWrap::CounterTypeString, napi_enumerable),
        StaticMethod("stringCounterType", &CounterWrap::StringCounterType, napi_enumerable),
        InstanceAccessor("value", &CounterWrap::GetValue, &CounterWrap::SetValue, napi_enumerable)
    };
    ctor = napi_init<CounterWrap, PROTOCORE_DOMAIN::ObjectWrap>(env, exports, "Counter", properties);
}

CounterWrap::CounterWrap(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<CounterWrap>(info)
    , d(napi_unwrap_data<Icd::CounterItem>(info, true))
{

}

ITEMWRAP_METHODS_IMPL(CounterWrap)

NAPI_GETTER(CounterWrap, CounterType) {
    return Napi::Number::New(info.Env(), d->counterType());
}

NAPI_SETTER(CounterWrap, CounterType) {
    d->setCounterType(Icd::CounterType(info[0].As<Napi::Number>().Int32Value()));
}

NAPI_METHOD(CounterWrap, CounterTypeString) {
    if (info.Length() != 1) {
        return napi_throwjs(Napi::ArgumentError::New(info.Env(), "Need one argument!"));
        return Napi::Value();
    }
    return Napi::String::New(info.Env(), Icd::CounterItem::counterTypeString
                             (Icd::CounterType(info[0].As<Napi::Number>().Int32Value())));
}

NAPI_METHOD(CounterWrap, StringCounterType) {
    if (info.Length() != 1) {
        return napi_throwjs(Napi::ArgumentError::New(info.Env(), "Need one argument!"));
        return Napi::Value();
    }
    return Napi::Number::New(info.Env(), Icd::CounterItem::stringCounterType(info[0].As<Napi::String>()));
}

NAPI_GETTER(CounterWrap, Value) {
    return Napi::Number::New(info.Env(), d->value());
}

NAPI_SETTER(CounterWrap, Value) {
    d->setValue((unsigned char)(info[0].As<Napi::Number>().Int32Value()));
}

PROTOCORE_END
