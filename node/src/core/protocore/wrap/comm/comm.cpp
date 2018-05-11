#include "precomp.h"
#include "comm.h"
#include "channel_wrap.h"
#include "filechannel_wrap.h"
#include "serialchannel_wrap.h"
#include "udpchannel_wrap.h"

PROTOCORE_BEGIN

void comm_init(Napi::Env env, Napi::Object exports)
{
    ChannelWrap::Initialize(env, exports);
    FileChannelWrap::Initialize(env, exports);
    SerialChannelWrap::Initialize(env, exports);
    UdpChannelWrap::Initialize(env, exports);
}

PROTOCORE_END
