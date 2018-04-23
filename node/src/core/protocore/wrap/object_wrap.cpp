#include "precomp.h"
#include "object_wrap.h"
#include "icdcore/icd_object.h"

PROTOCORE_BEGIN

Napi::FunctionReference BaseObject::ctor;

void BaseObject::Initialize(Napi::Env env, Napi::Object exports)
{
    std::vector<PropertyDescriptor> properties = {
        InstanceAccessor("objectType", &BaseObject::GetObjectType, nullptr),
        InstanceAccessor("id", &BaseObject::GetId, &BaseObject::SetId),
        InstanceAccessor("domain", &BaseObject::GetDomain, &BaseObject::SetDomain),
        InstanceAccessor("name", &BaseObject::GetName, &BaseObject::SetName),
        InstanceAccessor("mark", &BaseObject::GetMark, &BaseObject::SetMark),
        InstanceAccessor("desc", &BaseObject::GetDesc, &BaseObject::SetDesc),
        InstanceMethod("isPrivateMark", &BaseObject::IsPrivateMark),
        InstanceMethod("childCount", &BaseObject::ChildCount),
        InstanceMethod("objectTypeString", &BaseObject::ObjectTypeString),
        InstanceMethod("resetData", &BaseObject::ResetData),
        InstanceMethod("clearData", &BaseObject::ClearData),
        InstanceMethod("domainOfType", &BaseObject::DomainOfType)
    };
    ctor = napi_init<BaseObject>(env, exports, "Object", properties);
}

BaseObject::BaseObject(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<BaseObject>(info)
    , data(napi_unwrap_data<Icd::Object>(info, true))
{

}

NAPI_GETTER(BaseObject, ObjectType) {
    return Napi::Number::New(info.Env(), data->objectType());
}

NAPI_GETTER(BaseObject, Id) {
    return Napi::String::New(info.Env(), data->id());
}

NAPI_SETTER(BaseObject, Id) {
    data->setId(value.As<Napi::String>());
}

NAPI_GETTER(BaseObject, Domain) {
    return Napi::String::New(info.Env(), data->domain());
}

NAPI_SETTER(BaseObject, Domain) {
    data->setId(value.As<Napi::String>());
}

NAPI_GETTER(BaseObject, Name) {
    return Napi::String::New(info.Env(), data->name());
}

NAPI_SETTER(BaseObject, Name) {
    data->setName(value.As<Napi::String>());
}

NAPI_GETTER(BaseObject, Mark) {
    return Napi::String::New(info.Env(), data->mark());
}

NAPI_SETTER(BaseObject, Mark) {
    data->setMark(value.As<Napi::String>());
}

NAPI_GETTER(BaseObject, Desc) {
    return Napi::String::New(info.Env(), data->desc());
}

NAPI_SETTER(BaseObject, Desc) {
    data->setDesc(value.As<Napi::String>());
}

NAPI_METHOD(BaseObject, IsPrivateMark) {
    return Napi::Boolean::New(info.Env(), data->isPrivateMark());
}

NAPI_METHOD(BaseObject, ChildCount) {
    return Napi::Number::New(info.Env(), data->childCount());
}

NAPI_METHOD(BaseObject, ObjectTypeString) {
    return Napi::String::New(info.Env(), data->objectTypeString());
}

NAPI_VOID_METHOD(BaseObject, ResetData) {
    data->resetData();
}

NAPI_VOID_METHOD(BaseObject, ClearData) {
    data->clearData();
}

NAPI_METHOD(BaseObject, DomainOfType) {
    return Napi::String::New(info.Env(), data->domainOfType(info[0].As<Napi::Number>()));
}

PROTOCORE_END
