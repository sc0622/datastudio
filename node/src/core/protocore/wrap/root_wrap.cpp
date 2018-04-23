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
        InstanceAccessor("vehicles", &RootWrap::vehicles, nullptr)
    };
    ctor = napi_init<RootWrap, BaseObject>(env, exports, "Root", properties);
}

RootWrap::RootWrap(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<RootWrap>(info)
    , data(napi_unwrap_data<Icd::Root>(info, true))
{
    //test
    data->setMark("id::root");
    for (int i = 0; i < 3; ++i) {
        std::ostringstream oss;
        oss << "item " << i;
        Icd::VehiclePtr vehicle = Icd::VehiclePtr(new Icd::Vehicle(oss.str()));
        data->appendVehicle(vehicle);
    }
}

Napi::Value RootWrap::vehicles(const Napi::CallbackInfo &info)
{
    const auto &vehicles = data->allVehicle();
    Napi::Array items = Napi::Array::New(info.Env(), vehicles.size());
    for (int i = 0; i < vehicles.size(); ++i) {
        const Icd::VehiclePtr &vehicle = vehicles[i];
        if (vehicle) {
            items.Set(i, napi_instance(info.Env(), VehicleWrap::ctor, vehicle));
        }
    }
    return items;
}

PROTOCORE_END
