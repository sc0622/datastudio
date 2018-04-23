#include "precomp.h"
#include "vehicle_wrap.h"
#include "icdcore/icd_vehicle.h"

PROTOCORE_BEGIN

Napi::FunctionReference VehicleWrap::ctor;

void VehicleWrap::Initialize(Napi::Env env, Napi::Object exports)
{
    std::vector<PropertyDescriptor> properties = {
        //
    };
    ctor = napi_init<VehicleWrap, BaseObject>(env, exports, "Vehicle", properties);
}

VehicleWrap::VehicleWrap(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<VehicleWrap>(info)
    , data(napi_unwrap_data<Icd::Vehicle>(info))
{

}

PROTOCORE_END
