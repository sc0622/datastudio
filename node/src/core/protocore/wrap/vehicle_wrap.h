#pragma once

#include "node_global.h"
#include "object_wrap.h"

namespace Icd {
class Vehicle;
typedef std::shared_ptr<Icd::Vehicle> VehiclePtr;
}

PROTOCORE_BEGIN

class VehicleWrap : public Napi::ObjectWrap<VehicleWrap>
{
public:
    static Napi::FunctionReference ctor;
    static void Initialize(Napi::Env env, Napi::Object exports);
    VehicleWrap(const Napi::CallbackInfo &info);

private:
    Icd::VehiclePtr data;
};

PROTOCORE_END
