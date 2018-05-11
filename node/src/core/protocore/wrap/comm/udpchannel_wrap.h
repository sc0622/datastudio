#ifndef UDPCHANNEL_WRAP_H
#define UDPCHANNEL_WRAP_H

#include "channel_wrap.h"

namespace Icd {
class UdpChannel;
typedef std::shared_ptr<UdpChannel> UdpChannelPtr;
}

PROTOCORE_BEGIN

class UdpChannelWrap : public Napi::ObjectWrap<UdpChannelWrap>
{
public:
    static Napi::FunctionReference ctor;
    static void Initialize(Napi::Env env, Napi::Object exports);
    UdpChannelWrap(const Napi::CallbackInfo &info);

    Icd::UdpChannelPtr data() const { return d; }

    CHANNELWRAP_METHODS_DECL();
    NAPI_PROPERTY_DECL(OpenMode);
    NAPI_METHOD_DECL(SetConfig);
    NAPI_METHOD_DECL(IsMulticastIP);
    NAPI_PROPERTY_DECL(LocalAddr);
    NAPI_PROPERTY_DECL(LocalPort);
    NAPI_PROPERTY_DECL(RemoteAddr);
    NAPI_PROPERTY_DECL(RemotePort);

private:
    Icd::UdpChannelPtr d;
};

PROTOCORE_END

#endif // UDPCHANNEL_WRAP_H
