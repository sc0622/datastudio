#include "precomp.h"
#include "vehicle_wrap.h"
#include "system_wrap.h"
#include "icdcore/icd_vehicle.h"
#include "icdcore/icd_system.h"

PROTOCORE_BEGIN

Napi::FunctionReference VehicleWrap::ctor;

void VehicleWrap::Initialize(Napi::Env env, Napi::Object exports)
{
    std::vector<PropertyDescriptor> properties = {
        OBJECTWRAP_METHODS_PROPS(VehicleWrap),
        InstanceAccessor("systems", &VehicleWrap::GetSystems, nullptr, napi_enumerable),
        InstanceMethod("appendSystem", &VehicleWrap::AppendSystem, napi_enumerable),
        InstanceMethod("insertSystem", &VehicleWrap::InsertSystem, napi_enumerable),
        InstanceMethod("removeSystem", &VehicleWrap::RemoveSystem, napi_enumerable),
        InstanceMethod("removeSystemByMark", &VehicleWrap::RemoveSystemByMark, napi_enumerable),
        InstanceMethod("clearSystem", &VehicleWrap::ClearSystem, napi_enumerable),
        InstanceAccessor("systemCount", &VehicleWrap::GetSystemCount, nullptr, napi_enumerable),
        InstanceMethod("systemAt", &VehicleWrap::SystemAt, napi_enumerable),
        InstanceMethod("systemByMark", &VehicleWrap::SystemByMark, napi_enumerable)
    };
    ctor = napi_init<VehicleWrap, PROTOCORE_DOMAIN::ObjectWrap>(env, exports, "Vehicle", properties);
}

VehicleWrap::VehicleWrap(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<VehicleWrap>(info)
    , d(napi_unwrap_data<Icd::Vehicle>(info, true))
{

}

OBJECTWRAP_METHODS_IMPL(VehicleWrap)

NAPI_GETTER(VehicleWrap, Systems) {
    const auto &systems = d->allSystem();
    Napi::Array array = Napi::Array::New(info.Env(), systems.size());
    for (int i = 0; i < systems.size(); ++i) {
        const Icd::SystemPtr &system = systems[i];
        if (system) {
            array.Set(i, napi_instance(info.Env(), SystemWrap::ctor, system));
        }
    }
    return array;
}

NAPI_VOID_METHOD(VehicleWrap, AppendSystem) {
    if (info.Length() != 1) {
        NAPI_THROW(Napi::Error::New(info.Env(), "Need one argument!"));
        return;
    }
    const SystemWrap *system = SystemWrap::Unwrap(info[0].As<Napi::Object>());
    if (!system) {
        return;
    }
    d->appendSystem(system->data());
}

NAPI_VOID_METHOD(VehicleWrap, InsertSystem) {
    if (info.Length() != 2) {
        NAPI_THROW(Napi::Error::New(info.Env(), "Need two arguments!"));
        return;
    }
    const int index = info[0].As<Napi::Number>();
    const SystemWrap *system = SystemWrap::Unwrap(info[1].As<Napi::Object>());
    if (!system) {
        return;
    }
    d->insertSystem(index, system->data());
}

NAPI_VOID_METHOD(VehicleWrap, RemoveSystem) {
    if (info.Length() != 1) {
        NAPI_THROW(Napi::Error::New(info.Env(), "Need one argument!"));
        return;
    }
    d->removeSystem(info[0].As<Napi::Number>());
}

NAPI_VOID_METHOD(VehicleWrap, RemoveSystemByMark) {
    if (info.Length() != 1) {
        NAPI_THROW(Napi::Error::New(info.Env(), "Need one argument!"));
        return;
    }
    d->removeSystemByMark(info[0].As<Napi::String>());
}

NAPI_VOID_METHOD(VehicleWrap, ClearSystem) {
    d->clearSystem();
}

NAPI_GETTER(VehicleWrap, SystemCount) {
    return Napi::Number::New(info.Env(), d->systemCount());
}

NAPI_METHOD(VehicleWrap, SystemAt) {
    if (info.Length() != 1) {
        NAPI_THROW(Napi::Error::New(info.Env(), "Need one argument!"));
        return Napi::Value();
    }
    auto system = d->systemAt(info[0].As<Napi::Number>());
    if (system) {
        return napi_instance(info.Env(), VehicleWrap::ctor, system);
    }

    return Napi::Value();
}

NAPI_METHOD(VehicleWrap, SystemByMark) {
    if (info.Length() != 1) {
        NAPI_THROW(Napi::Error::New(info.Env(), "Need one argument!"));
        return Napi::Value();
    }
    auto system = d->systemByMark(info[0].As<Napi::String>());
    if (system) {
        return napi_instance(info.Env(), VehicleWrap::ctor, system);
    }

    return Napi::Value();
}

PROTOCORE_END
