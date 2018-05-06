#include "precomp.h"
#include "object_wrap.h"
#include "icdcore/icd_object.h"

PROTOCORE_BEGIN

Napi::FunctionReference ObjectWrap::ctor;

void ObjectWrap::Initialize(Napi::Env env, Napi::Object exports)
{
    std::vector<PropertyDescriptor> properties = {
        InstanceAccessor("objectType", &ObjectWrap::GetObjectType, nullptr, napi_enumerable),
        InstanceAccessor("id", &ObjectWrap::GetId, &ObjectWrap::SetId, napi_enumerable),
        InstanceAccessor("domain", &ObjectWrap::GetDomain, &ObjectWrap::SetDomain, napi_enumerable),
        InstanceAccessor("name", &ObjectWrap::GetName, &ObjectWrap::SetName, napi_enumerable),
        InstanceAccessor("mark", &ObjectWrap::GetMark, &ObjectWrap::SetMark, napi_enumerable),
        InstanceAccessor("desc", &ObjectWrap::GetDesc, &ObjectWrap::SetDesc, napi_enumerable),
        InstanceAccessor("isPrivateMark", &ObjectWrap::GetIsPrivateMark, nullptr, napi_enumerable),
        InstanceAccessor("childCount", &ObjectWrap::GetChildCount, nullptr, napi_enumerable),
        InstanceAccessor("objectTypeString", &ObjectWrap::GetObjectTypeString, nullptr, napi_enumerable),
        OBJECTWRAP_METHODS_PROPS(ObjectWrap)
    };
    ctor = napi_init<ObjectWrap>(env, exports, "Object", properties);
}

ObjectWrap::ObjectWrap(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<ObjectWrap>(info)
    , d(napi_unwrap_data<Icd::Object>(info, true))
{

}

NAPI_GETTER(ObjectWrap, ObjectType) {
    return Napi::Number::New(info.Env(), d->objectType());
}

NAPI_GETTER(ObjectWrap, Id) {
    return Napi::String::New(info.Env(), d->id());
}

NAPI_SETTER(ObjectWrap, Id) {
    d->setId(value.As<Napi::String>());
}

NAPI_GETTER(ObjectWrap, Domain) {
    return Napi::String::New(info.Env(), d->domain());
}

NAPI_SETTER(ObjectWrap, Domain) {
    d->setDomain(value.As<Napi::String>());
}

NAPI_GETTER(ObjectWrap, Name) {
    return Napi::String::New(info.Env(), d->name());
}

NAPI_SETTER(ObjectWrap, Name) {
    d->setName(value.As<Napi::String>());
}

NAPI_GETTER(ObjectWrap, Mark) {
    return Napi::String::New(info.Env(), d->mark());
}

NAPI_SETTER(ObjectWrap, Mark) {
    d->setMark(value.As<Napi::String>());
}

NAPI_GETTER(ObjectWrap, Desc) {
    return Napi::String::New(info.Env(), d->desc());
}

NAPI_SETTER(ObjectWrap, Desc) {
    d->setDesc(value.As<Napi::String>());
}

NAPI_GETTER(ObjectWrap, IsPrivateMark) {
    return Napi::Boolean::New(info.Env(), d->isPrivateMark());
}

NAPI_GETTER(ObjectWrap, ChildCount) {
    return Napi::Number::New(info.Env(), d->childCount());
}

NAPI_GETTER(ObjectWrap, ObjectTypeString) {
    return Napi::String::New(info.Env(), d->objectTypeString());
}

OBJECTWRAP_METHODS_IMPL(ObjectWrap)

PROTOCORE_END
