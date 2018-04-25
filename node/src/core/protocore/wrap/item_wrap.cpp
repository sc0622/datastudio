#include "precomp.h"
#include "item_wrap.h"
#include "icdcore/icd_item.h"

PROTOCORE_BEGIN

Napi::FunctionReference ItemWrap::ctor;

void ItemWrap::Initialize(Napi::Env env, Napi::Object exports)
{
    std::vector<PropertyDescriptor> properties = {
        InstanceAccessor("type", &ItemWrap::GetType, nullptr, napi_enumerable),
        //InstanceAccessor("buffer", &ItemWrap::GetBuffer, nullptr, napi_enumerable),
        InstanceAccessor("bufferSize", &ItemWrap::GetBufferSize, nullptr, napi_enumerable),
        InstanceAccessor("bufferOffset", &ItemWrap::GetBufferOffset, nullptr, napi_enumerable),
        InstanceAccessor("data", &ItemWrap::GetData, &ItemWrap::SetData, napi_enumerable),
        InstanceAccessor("dataString", &ItemWrap::GetDataString, nullptr, napi_enumerable),
        InstanceAccessor("defaultValue", &ItemWrap::GetDefaultValue, nullptr, napi_enumerable),
        InstanceAccessor("typeName", &ItemWrap::GetTypeName, nullptr, napi_enumerable),
        InstanceAccessor("codeName", &ItemWrap::GetCodeName, nullptr, napi_enumerable),
        StaticMethod("create", &ItemWrap::Create, napi_enumerable),
        ITEMWRAP_METHODS_PROPS(ItemWrap),
    };
    ctor = napi_init<ItemWrap, PROTOCORE_DOMAIN::ObjectWrap>(env, exports, "Item", properties);
}

ItemWrap::ItemWrap(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<ItemWrap>(info)
    , d(napi_unwrap_data<Icd::Item>(info, true))
{

}

NAPI_GETTER(ItemWrap, Type) {
    return Napi::Number::New(info.Env(), d->type());
}

NAPI_GETTER(ItemWrap, BufferSize) {
    return Napi::Number::New(info.Env(), d->bufferSize());
}

NAPI_GETTER(ItemWrap, BufferOffset) {
    return Napi::Number::New(info.Env(), d->bufferOffset());
}

NAPI_GETTER(ItemWrap, Data) {
    return Napi::Number::New(info.Env(), d->data());
}

NAPI_SETTER(ItemWrap, Data) {
    d->setData(info[0].As<Napi::Number>());
}

NAPI_GETTER(ItemWrap, DataString) {
    return Napi::String::New(info.Env(), d->dataString());
}

NAPI_GETTER(ItemWrap, DefaultValue) {
    return Napi::Number::New(info.Env(), d->defaultValue());
}

NAPI_SETTER(ItemWrap, DefaultValue) {
    d->setDefaultValue(info[0].As<Napi::Number>());
}

NAPI_GETTER(ItemWrap, TypeName) {
    return Napi::String::New(info.Env(), d->typeName());
}

NAPI_GETTER(ItemWrap, CodeName) {
    return Napi::String::New(info.Env(), d->codeName());
}

NAPI_METHOD(ItemWrap, Create) {
    if (info.Length() != 2) {
        NAPI_THROW(Napi::Error::New(info.Env(), "Need two arguments as least!"));
        return Napi::Value();
    }
    auto item = Icd::Item::create(info[0].As<Napi::String>(),
            Icd::ItemType(info[0].As<Napi::Number>().Int32Value()));
    if (item) {
        return napi_instance(info.Env(), ItemWrap::ctor, item);
    }

    return Napi::Value();
}

ITEMWRAP_METHODS_IMPL(ItemWrap)

PROTOCORE_END
