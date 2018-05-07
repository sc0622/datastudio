#include "precomp.h"
#include "Numeric_wrap.h"
#include "icdcore/icd_item_numeric.h"
#include "icdcore/icd_item_limit.h"
#include "limit_wrap.h"

PROTOCORE_BEGIN

Napi::FunctionReference NumericWrap::ctor;

void NumericWrap::Initialize(Napi::Env env, Napi::Object exports)
{
    std::vector<PropertyDescriptor> properties = {
        ITEMWRAP_METHODS_PROPS(NumericWrap),
        InstanceAccessor("numericType", &NumericWrap::GetNumericType, &NumericWrap::SetNumericType, napi_enumerable),
        InstanceAccessor("numericTypeString", &NumericWrap::GetNumericTypeString, nullptr, napi_enumerable),
        StaticMethod("numericTypeString", &NumericWrap::NumericTypeString, napi_enumerable),
        StaticMethod("stringNumericType", &NumericWrap::StringNumericType, napi_enumerable),
        InstanceAccessor("originalData", &NumericWrap::GetOriginalData, nullptr, napi_enumerable),
        InstanceAccessor("scale", &NumericWrap::GetScale, &NumericWrap::SetScale, napi_enumerable),
        InstanceAccessor("decimals", &NumericWrap::GetDecimals, &NumericWrap::SetDecimals, napi_enumerable),
        InstanceAccessor("offset", &NumericWrap::GetOffset, &NumericWrap::SetOffset, napi_enumerable),
        InstanceAccessor("limit", &NumericWrap::GetLimit, &NumericWrap::SetLimit, napi_enumerable),
        InstanceAccessor("unit", &NumericWrap::GetUnit, &NumericWrap::SetUnit, napi_enumerable),
        InstanceAccessor("specs", &NumericWrap::GetSpecs, nullptr, napi_enumerable),
        InstanceMethod("addSpec", &NumericWrap::AddSpec, napi_enumerable),
        InstanceMethod("specAt", &NumericWrap::SpecAt, napi_enumerable),
        InstanceMethod("removeSpec", &NumericWrap::RemoveSpec, napi_enumerable),
        InstanceMethod("clearSpec", &NumericWrap::ClearSpec, napi_enumerable),
        InstanceAccessor("dataRange", &NumericWrap::GetDataRange, nullptr, napi_enumerable),
        InstanceAccessor("valueRange", &NumericWrap::GetValueRange, nullptr, napi_enumerable),
        InstanceAccessor("outOfLimit", &NumericWrap::GetOutOfLimit, nullptr, napi_enumerable)
    };
    ctor = napi_init<NumericWrap, PROTOCORE_DOMAIN::ObjectWrap>(env, exports, "Numeric", properties);
}

NumericWrap::NumericWrap(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<NumericWrap>(info)
    , d(napi_unwrap_data<Icd::NumericItem>(info, true))
{
    QString s("what the...");
    d->setMark(s.toStdString());
}

ITEMWRAP_METHODS_IMPL(NumericWrap)

NAPI_GETTER(NumericWrap, NumericType) {
    return Napi::Number::New(info.Env(), d->numericType());
}

NAPI_SETTER(NumericWrap, NumericType) {
    d->setNumericType(Icd::NumericType(info[0].As<Napi::Number>().Int32Value()));
}

NAPI_GETTER(NumericWrap, NumericTypeString) {
    return Napi::String::New(info.Env(), d->numericTypeString());
}

NAPI_METHOD(NumericWrap, NumericTypeString) {
    if (info.Length() != 1) {
        NAPI_THROW(Napi::Error::New(info.Env(), "Need one argument!"));
        return Napi::Value();
    }
    return Napi::String::New(info.Env(), Icd::NumericItem::numericTypeString
                             (Icd::NumericType(info[0].As<Napi::Number>().Int32Value())));
}

NAPI_METHOD(NumericWrap, StringNumericType) {
    if (info.Length() != 1) {
        NAPI_THROW(Napi::Error::New(info.Env(), "Need one argument!"));
        return Napi::Value();
    }
    return Napi::Number::New(info.Env(), Icd::NumericItem::stringNumericType(info[0].As<Napi::String>()));
}

NAPI_GETTER(NumericWrap, OriginalData) {
    return Napi::Number::New(info.Env(), d->originalData());
}

NAPI_GETTER(NumericWrap, Scale) {
    return Napi::Number::New(info.Env(), d->scale());
}

NAPI_SETTER(NumericWrap, Scale) {
    d->setScale(info[0].As<Napi::Number>());
}

NAPI_GETTER(NumericWrap, Decimals) {
    return Napi::Number::New(info.Env(), d->decimals());
}

NAPI_SETTER(NumericWrap, Decimals) {
    d->setDecimals(info[0].As<Napi::Number>());
}

NAPI_GETTER(NumericWrap, Offset) {
    return Napi::Number::New(info.Env(), d->offset());
}

NAPI_SETTER(NumericWrap, Offset) {
    d->setOffset(info[0].As<Napi::Number>());
}

NAPI_GETTER(NumericWrap, Limit) {
    return napi_instance(info.Env(), LimitWrap::ctor, d->limit());
}

NAPI_SETTER(NumericWrap, Limit) {
    auto limitWrap = LimitWrap::Unwrap(info[0].As<Napi::Object>());
    if (!limitWrap) {
        NAPI_THROW(Napi::TypeError::New(info.Env(), "Need a LimitWrap argument!"));
        return;
    }
    d->setLimit(limitWrap->data());
}

NAPI_GETTER(NumericWrap, Unit) {
    return Napi::String::New(info.Env(), d->unit());
}

NAPI_SETTER(NumericWrap, Unit) {
    d->setUnit(info[0].As<Napi::String>());
}

NAPI_GETTER(NumericWrap, Specs) {
    const auto &specs = d->specs();
    Napi::Object object = Napi::Object::New(info.Env());
    for (auto citer = specs.cbegin(); citer != specs.cend(); ++citer) {
        object.Set(citer->first, Napi::String::New(info.Env(), citer->second));
    }
    return object;
}

NAPI_VOID_METHOD(NumericWrap, AddSpec) {
    if (info.Length() != 2) {
        NAPI_THROW(Napi::Error::New(info.Env(), "Need two arguments!"));
        return;
    }
    d->addSpec(info[0].As<Napi::Number>(), info[1].As<Napi::String>());
}

NAPI_METHOD(NumericWrap, SpecAt) {
    if (info.Length() != 1) {
        NAPI_THROW(Napi::Error::New(info.Env(), "Need one argument!"));
        return Napi::Value();
    }
    return Napi::String::New(info.Env(), d->specAt(info[0].As<Napi::Number>()));
}

NAPI_VOID_METHOD(NumericWrap, RemoveSpec) {
    if (info.Length() != 1) {
        NAPI_THROW(Napi::Error::New(info.Env(), "Need one argument!"));
        return;
    }
    d->removeSpec(info[0].As<Napi::Number>());
}

NAPI_VOID_METHOD(NumericWrap, ClearSpec) {
    d->clearSpec();
}

NAPI_GETTER(NumericWrap, DataRange) {
    Napi::Array array = Napi::Array::New(info.Env(), 2);
    const auto &range = d->dataRange();
    array.Set(uint32_t(0), Napi::Number::New(info.Env(), range.first));
    array.Set(uint32_t(1), Napi::Number::New(info.Env(), range.second));
    return array;
}

NAPI_GETTER(NumericWrap, ValueRange) {
    Napi::Array array = Napi::Array::New(info.Env(), 2);
    const auto &range = d->valueRange();
    array.Set(uint32_t(0), Napi::Number::New(info.Env(), range.first));
    array.Set(uint32_t(1), Napi::Number::New(info.Env(), range.second));
    return array;
}

NAPI_GETTER(NumericWrap, OutOfLimit) {
    return Napi::Boolean::New(info.Env(), d->outOfLimit());
}

PROTOCORE_END
