#pragma once

#include "node_global.h"
#include "object_wrap.h"

namespace Icd {
class Vehicle;
typedef std::shared_ptr<Vehicle> VehiclePtr;
}

PROTOCORE_BEGIN

class VehicleWrap : public Napi::ObjectWrap<VehicleWrap>
{
public:
    static Napi::FunctionReference ctor;
    static void Initialize(Napi::Env env, Napi::Object exports);
    VehicleWrap(const Napi::CallbackInfo &info);

    Icd::VehiclePtr data() const { return d; }

    OBJECTWRAP_METHODS_DECL();

    NAPI_GETTER_DECL(Systems);
    NAPI_VOID_METHOD_DECL(AppendSystem);
    NAPI_VOID_METHOD_DECL(InsertSystem);
    NAPI_VOID_METHOD_DECL(RemoveSystem);
    NAPI_VOID_METHOD_DECL(RemoveSystemByMark);
    NAPI_VOID_METHOD_DECL(ClearSystem);
    NAPI_GETTER_DECL(SystemCount);
    NAPI_METHOD_DECL(SystemAt);
    NAPI_METHOD_DECL(SystemByMark);

private:
    Icd::VehiclePtr d;
};

PROTOCORE_END
