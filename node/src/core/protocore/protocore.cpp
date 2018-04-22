
#include "node_global.h"
#include "wrap/object_wrap.h"
#include "wrap/root_wrap.h"
#include "wrap/vehicle_wrap.h"

PROTOCORE_BEGIN

Napi::Object Initialize(Napi::Env env, Napi::Object exports)
{
    BaseObject::Initialize(env, exports);
    VehicleWrap::Initialize(env, exports);
    RootWrap::Initialize(env, exports);
    return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Initialize)

PROTOCORE_END
