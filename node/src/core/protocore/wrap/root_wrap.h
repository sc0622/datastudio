#pragma once

#include "node_global.h"
#include "object_wrap.h"

namespace Icd {
class Root;
typedef std::shared_ptr<Root> RootPtr;
}

PROTOCORE_BEGIN

class RootWrap : public Napi::ObjectWrap<RootWrap>
{
public:
    static Napi::FunctionReference ctor;
    static void Initialize(Napi::Env env, Napi::Object exports);
    RootWrap(const Napi::CallbackInfo &info);

    Icd::RootPtr data() const { return d; }

    OBJECTWRAP_METHODS_DECL();

    NAPI_GETTER_DECL(Vehicles);
    NAPI_VOID_METHOD_DECL(AppendVehicle);
    NAPI_VOID_METHOD_DECL(InsertVehicle);
    NAPI_VOID_METHOD_DECL(RemoveVehicle);
    NAPI_VOID_METHOD_DECL(RemoveVehicleByMark);
    NAPI_VOID_METHOD_DECL(ClearVehicle);
    NAPI_GETTER_DECL(VehicleCount);
    NAPI_METHOD_DECL(VehicleAt);
    NAPI_METHOD_DECL(VehicleByMark);

private:
    Icd::RootPtr d;
};

PROTOCORE_END
