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
    VehicleWrap(const Napi::CallbackInfo &info);

    static void Initialize(Napi::Env env, Napi::Object exports);

private:
    Icd::VehiclePtr data;
    Napi::FunctionReference Constructor;
};

PROTOCORE_END
