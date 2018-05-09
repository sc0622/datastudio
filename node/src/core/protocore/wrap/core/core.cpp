#include "precomp.h"
#include "core.h"
#include "./object_wrap.h"
#include "./root_wrap.h"
#include "./vehicle_wrap.h"
#include "./system_wrap.h"
#include "./table_wrap.h"
#include "./item_wrap.h"
#include "./bit_wrap.h"
#include "./check_wrap.h"
#include "./complex_wrap.h"
#include "./counter_wrap.h"
#include "./frame_wrap.h"
#include "./framecode_wrap.h"
#include "./header_wrap.h"
#include "./limit_wrap.h"
#include "./numeric_wrap.h"

PROTOCORE_BEGIN

void core_init(Napi::Env env, Napi::Object exports)
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
}

PROTOCORE_END
