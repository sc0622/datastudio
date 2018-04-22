#include "precomp.h"
#include "vehicle_wrap.h"
#include "icdcore/icd_vehicle.h"

PROTOCORE_BEGIN

VehicleWrap::VehicleWrap(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<VehicleWrap>(info)
{

}

void VehicleWrap::Initialize(Napi::Env env, Napi::Object exports)
{
    std::vector<PropertyDescriptor> properties = {
        //
    };
    exports.Set("Vehicle", DefineClass(env, "Vehicle", properties));
}

PROTOCORE_END
