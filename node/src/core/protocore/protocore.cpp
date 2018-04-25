#include "node_global.h"
#include "wrap/object_wrap.h"
#include "wrap/root_wrap.h"
#include "wrap/vehicle_wrap.h"
#include "wrap/system_wrap.h"
#include "wrap/table_wrap.h"
#include "wrap/item_wrap.h"
#include "wrap/bit_wrap.h"
#include "wrap/check_wrap.h"
#include "wrap/complex_wrap.h"
#include "wrap/counter_wrap.h"
#include "wrap/frame_wrap.h"
#include "wrap/framecode_wrap.h"
#include "wrap/header_wrap.h"
#include "wrap/limit_wrap.h"
#include "wrap/numeric_wrap.h"

PROTOCORE_BEGIN

Napi::Object Initialize(Napi::Env env, Napi::Object exports)
{
    ObjectWrap::Initialize(env, exports);
    RootWrap::Initialize(env, exports);
    VehicleWrap::Initialize(env, exports);
    SystemWrap::Initialize(env, exports);
    TableWrap::Initialize(env, exports);
    ItemWrap::Initialize(env, exports);
    BitWrap::Initialize(env, exports);
    CheckWrap::Initialize(env, exports);
    ComplexWrap::Initialize(env, exports);
    CounterWrap::Initialize(env, exports);
    FrameWrap::Initialize(env, exports);
    FrameCodeWrap::Initialize(env, exports);
    HeaderWrap::Initialize(env, exports);
    LimitWrap::Initialize(env, exports);
    NumericWrap::Initialize(env, exports);
    return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Initialize)

PROTOCORE_END
