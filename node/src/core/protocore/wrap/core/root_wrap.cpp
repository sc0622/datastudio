#include "precomp.h"
#include "root_wrap.h"
#include "vehicle_wrap.h"
#include "icdcore/icd_root.h"
#include "icdcore/icd_vehicle.h"

PROTOCORE_BEGIN

Napi::FunctionReference RootWrap::ctor;

void RootWrap::Initialize(Napi::Env env, Napi::Object exports)
{
    std::vector<PropertyDescriptor> properties = {
        OBJECTWRAP_METHODS_PROPS(RootWrap),
        InstanceAccessor("vehicles", &RootWrap::GetVehicles, nullptr, napi_enumerable),
        InstanceMethod("appendVehicle", &RootWrap::AppendVehicle, napi_enumerable),
        InstanceMethod("insertVehicle", &RootWrap::InsertVehicle, napi_enumerable),
        InstanceMethod("removeVehicle", &RootWrap::RemoveVehicle, napi_enumerable),
        InstanceMethod("removeVehicleByMark", &RootWrap::RemoveVehicleByMark, napi_enumerable),
        InstanceMethod("clearVehicle", &RootWrap::ClearVehicle, napi_enumerable),
        InstanceAccessor("vehicleCount", &RootWrap::GetVehicleCount, nullptr, napi_enumerable),
        InstanceMethod("vehicleAt", &RootWrap::VehicleAt, napi_enumerable),
        InstanceMethod("vehicleByMark", &RootWrap::VehicleByMark, napi_enumerable)
    };
    ctor = napi_init<RootWrap, PROTOCORE_DOMAIN::ObjectWrap>(env, exports, "Root", properties);
}

RootWrap::RootWrap(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<RootWrap>(info)
    , d(napi_unwrap_data<Icd::Root>(info, true))
{

}

OBJECTWRAP_METHODS_IMPL(RootWrap)

NAPI_GETTER(RootWrap, Vehicles) {
    const auto &vehicles = d->allVehicle();
    Napi::Array items = Napi::Array::New(info.Env(), vehicles.size());
    for (int i = 0; i < vehicles.size(); ++i) {
        const Icd::VehiclePtr &vehicle = vehicles[i];
        if (vehicle) {
            items.Set(i, napi_instance(info.Env(), VehicleWrap::ctor, vehicle));
        }
    }
    return items;
}

NAPI_VOID_METHOD(RootWrap, AppendVehicle) {
    if (info.Length() != 1) {
        napi_throwjs(Napi::ArgumentError::New(info.Env(), "Need one argument!"));
        return;
    }
    const VehicleWrap *vehicle = VehicleWrap::Unwrap(info[0].As<Napi::Object>());
    if (!vehicle) {
        return;
    }
    d->appendVehicle(vehicle->data());
}

NAPI_VOID_METHOD(RootWrap, InsertVehicle) {
    if (info.Length() != 2) {
        napi_throwjs(Napi::ArgumentError::New(info.Env(), "Need two arguments!"));
        return;
    }
    const int index = info[0].As<Napi::Number>();
    const VehicleWrap *vehicle = VehicleWrap::Unwrap(info[1].As<Napi::Object>());
    if (!vehicle) {
        return;
    }
    d->insertVehicle(index, vehicle->data());
}

NAPI_VOID_METHOD(RootWrap, RemoveVehicle) {
    if (info.Length() != 1) {
        napi_throwjs(Napi::ArgumentError::New(info.Env(), "Need one argument!"));
        return;
    }
    d->removeVehicle(info[0].As<Napi::Number>());
}

NAPI_VOID_METHOD(RootWrap, RemoveVehicleByMark) {
    if (info.Length() != 1) {
        napi_throwjs(Napi::ArgumentError::New(info.Env(), "Need one argument!"));
        return;
    }
    d->removeVehicleByMark(info[0].As<Napi::String>());
}

NAPI_VOID_METHOD(RootWrap, ClearVehicle) {
    d->clearVehicle();
}

NAPI_GETTER(RootWrap, VehicleCount) {
    return Napi::Number::New(info.Env(), d->vehicleCount());
}

NAPI_METHOD(RootWrap, VehicleAt) {
    if (info.Length() != 1) {
        return napi_throwjs(Napi::ArgumentError::New(info.Env(), "Need one argument!"));
    }
    auto vehicle = d->vehicleAt(info[0].As<Napi::Number>());
    if (vehicle) {
        return napi_instance(info.Env(), VehicleWrap::ctor, vehicle);
    }

    return Napi::Value();
}

NAPI_METHOD(RootWrap, VehicleByMark) {
    if (info.Length() != 1) {
        return napi_throwjs(Napi::ArgumentError::New(info.Env(), "Need one argument!"));
    }
    auto vehicle = d->vehicleByMark(info[0].As<Napi::String>());
    if (vehicle) {
        return napi_instance(info.Env(), VehicleWrap::ctor, vehicle);
    }

    return Napi::Value();
}

PROTOCORE_END
