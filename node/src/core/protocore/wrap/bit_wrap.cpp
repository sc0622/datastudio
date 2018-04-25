#include "precomp.h"
#include "bit_wrap.h"
#include "icdcore/icd_item_bit.h"

PROTOCORE_BEGIN

Napi::FunctionReference BitWrap::ctor;

void BitWrap::Initialize(Napi::Env env, Napi::Object exports)
{
    std::vector<PropertyDescriptor> properties = {
        ITEMWRAP_METHODS_PROPS(BitWrap),
        InstanceMethod("testBit", &BitWrap::TestBit, napi_enumerable),
        InstanceAccessor("bitStart", &BitWrap::GetBitStart, &BitWrap::SetBitStart, napi_enumerable),
        InstanceAccessor("bitCount", &BitWrap::GetBitCount, &BitWrap::SetBitCount, napi_enumerable),
        InstanceAccessor("typeSize", &BitWrap::GetTypeSize, &BitWrap::SetTypeSize, napi_enumerable),
        InstanceAccessor("mask", &BitWrap::GetMask, nullptr, napi_enumerable),
        InstanceAccessor("specs", &BitWrap::GetSpecs, nullptr, napi_enumerable),
        InstanceMethod("addSpec", &BitWrap::AddSpec, napi_enumerable),
        InstanceMethod("removeSpec", &BitWrap::RemoveSpec, napi_enumerable),
        InstanceMethod("clearSpec", &BitWrap::ClearSpec, napi_enumerable),
        InstanceMethod("specAt", &BitWrap::SpecAt, napi_enumerable),
        InstanceMethod("nameAt", &BitWrap::NameAt, napi_enumerable),
        InstanceMethod("nameOf", &BitWrap::NameOf, napi_enumerable),
        InstanceMethod("descAt", &BitWrap::DescAt, napi_enumerable)
    };
    ctor = napi_init<BitWrap, PROTOCORE_DOMAIN::ObjectWrap>(env, exports, "Bit", properties);
}

BitWrap::BitWrap(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<BitWrap>(info)
    , d(napi_unwrap_data<Icd::BitItem>(info, true))
{

}

ITEMWRAP_METHODS_IMPL(BitWrap)

NAPI_METHOD(BitWrap, TestBit) {
    if (info.Length() != 1) {
        NAPI_THROW(Napi::Error::New(info.Env(), "Need one argument!"));
        return Napi::Value();
    }
    return Napi::Boolean::New(info.Env(), d->testBit(info[0].As<Napi::Number>().Int32Value()));
}

NAPI_GETTER(BitWrap, BitStart) {
    return Napi::Number::New(info.Env(), d->bitStart());
}

NAPI_SETTER(BitWrap, BitStart) {
    d->setBitStart(info[0].As<Napi::Number>());
}

NAPI_GETTER(BitWrap, BitCount) {
    return Napi::Number::New(info.Env(), d->bitCount());
}

NAPI_SETTER(BitWrap, BitCount) {
    d->setBitCount(info[0].As<Napi::Number>());
}

NAPI_GETTER(BitWrap, TypeSize) {
    return Napi::Number::New(info.Env(), d->typeSize());
}

NAPI_SETTER(BitWrap, TypeSize) {
    d->setTypeSize(info[0].As<Napi::Number>());
}

NAPI_GETTER(BitWrap, Mask) {
    return Napi::Number::New(info.Env(), d->mask());
}

NAPI_GETTER(BitWrap, Specs) {
    const auto &specs = d->specs();
    Napi::Array array = Napi::Array::New(info.Env());
    for (auto citer = specs.cbegin(); citer != specs.cend(); ++citer) {
        array.Set(citer->first, Napi::String::New(info.Env(), citer->second));
    }
    return array;
}

NAPI_VOID_METHOD(BitWrap, AddSpec) {
    if (info.Length() != 2) {
        NAPI_THROW(Napi::Error::New(info.Env(), "Need two arguments!"));
        return;
    }
    d->addSpec(info[0].As<Napi::Number>().Int64Value(), info[1].As<Napi::String>());
}

NAPI_VOID_METHOD(BitWrap, RemoveSpec) {
    if (info.Length() != 1) {
        NAPI_THROW(Napi::Error::New(info.Env(), "Need one argument!"));
        return;
    }
    d->removeSpec(info[0].As<Napi::Number>().Int64Value());
}

NAPI_VOID_METHOD(BitWrap, ClearSpec) {
    d->clearSpec();
}

NAPI_METHOD(BitWrap, SpecAt) {
    if (info.Length() != 1) {
        NAPI_THROW(Napi::Error::New(info.Env(), "Need one argument!"));
        return Napi::Value();
    }
    return Napi::String::New(info.Env(), d->specAt(info[0].As<Napi::Number>().Int64Value()));
}

NAPI_METHOD(BitWrap, NameAt) {
    if (info.Length() != 1) {
        NAPI_THROW(Napi::Error::New(info.Env(), "Need one argument!"));
        return Napi::Value();
    }
    return Napi::String::New(info.Env(), d->nameAt(info[0].As<Napi::Number>()));
}

NAPI_METHOD(BitWrap, NameOf) {
    if (info.Length() != 1) {
        NAPI_THROW(Napi::Error::New(info.Env(), "Need one argument!"));
        return Napi::Value();
    }
    return Napi::String::New(info.Env(), d->nameOf(info[0].As<Napi::String>()));
}

NAPI_METHOD(BitWrap, DescAt) {
    if (info.Length() != 1) {
        NAPI_THROW(Napi::Error::New(info.Env(), "Need one argument!"));
        return Napi::Value();
    }
    return Napi::String::New(info.Env(), d->descAt(info[0].As<Napi::Number>()));
}

PROTOCORE_END
