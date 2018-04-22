#include "precomp.h"
#include "root_wrap.h"
#include "vehicle_wrap.h"
#include "icdcore/icd_root.h"
#include "icdcore/icd_vehicle.h"

PROTOCORE_BEGIN

RootWrap::RootWrap(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<RootWrap>(info)
{
    //test
    for (int i = 0; i < 3; ++i) {
        std::ostringstream oss;
        oss << "item " << i;
        Icd::VehiclePtr vehicle = Icd::VehiclePtr(new Icd::Vehicle(oss.str()));
        data->appendVehicle(vehicle);
    }
    //
}

Napi::Value RootWrap::vehicles(const Napi::CallbackInfo &info)
{
    Q_UNUSED(info);
    if (data) {
        return Napi::Value();
    } else {
        return Napi::Value();
    }
}

void RootWrap::Initialize(Napi::Env env, Napi::Object exports)
{
    std::vector<PropertyDescriptor> properties = {
        //
    };
    exports.Set("Root", DefineClass(env, "Root", properties));
}

PROTOCORE_END
