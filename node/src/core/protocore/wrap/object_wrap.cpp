#include "precomp.h"
#include "object_wrap.h"
#include "icdcore/icd_object.h"

PROTOCORE_BEGIN

BaseObject::BaseObject(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<BaseObject>(info)
    , data(parse_object_data<Icd::Object>(info.Data(), true))
{

}

Napi::Value BaseObject::objectType(const Napi::CallbackInfo &info)
{
    if (data) {
        return Napi::Number::New(info.Env(), data->objectType());
    } else {
        return Napi::Value();
    }
}

void BaseObject::Initialize(Napi::Env env, Napi::Object exports)
{
    std::vector<PropertyDescriptor> properties = {
        InstanceAccessor("objectType", &BaseObject::objectType, nullptr)
    };
    exports.Set("Object", DefineClass(env, "Object", properties));
}

Napi::Object BaseObject::New(Napi::Env env, const Icd::ObjectPtr &data)
{
    Napi::ObjectWrap::New()
}

PROTOCORE_END
